#include "boot/fdt.h"
#include "boot/limine_responses.h"
#include "common_defs.h"
#include "debug/panic.h"
#include "error.h"
#include "limine.h"
#include "libfdt.h"
#include "memlayout.h"
#include "mm/kheap.h"
#include "sync/spinlock.h"

typedef struct {
	SpinLock lock;
	void *fdt;
} FDT;

static FDT fdt = {};

void fdt_init(void)
{
	INFO("Initializing flat device tree parser");
	spinlock_init(&fdt.lock, "fdt");
	struct limine_dtb_response *dtb_response = limine_device_tree();
	if (nullptr == dtb_response || nullptr == dtb_response->dtb_ptr) {
		panic("dtb is null");
	}

	void *dtb_ptr = dtb_response->dtb_ptr;

	i32 ret = fdt_check_header(dtb_ptr);
	if (0 != ret) {
		panic("fdt's header is corrupted: %s", fdt_strerror(ret));
	}

	usize total_size = fdt_totalsize(dtb_ptr);
	DEBUG("fdt total_sizep = %p, pages = %d", total_size,
	      total_size / PAGE_SIZE);
	ret = fdt_check_full(dtb_ptr, total_size);
	if (0 != ret) {
		panic("fdt's body is corrupted: %s", fdt_strerror(ret));
	}

	usize alloc_size = round_up(total_size, PAGE_SIZE);
	fdt.fdt = kalloc(alloc_size * 3);
	if (IS_ERR(fdt.fdt)) {
		panic("out of memory");
	}

	DEBUG("fullsize = %d", total_size);
	memcpy(fdt.fdt, dtb_ptr, total_size);
}

static const void *fdt_query_prop_value(i32 node_offset, const i8 *prop,
					i32 *len)
{
	return fdt_getprop(fdt.fdt, node_offset, prop, len);
}

bool fdt_get_reg_for_compat(const i8 *compat, Reg *reg)
{
	spinlock_acquire(&fdt.lock);
	i32 node = fdt_query_compat(compat);
	if (node < 0) {
		return false;
	}
	spinlock_release(&fdt.lock);
	return fdt_get_reg(node, reg);
}

i32 fdt_query_compat(const i8 *compat)
{
	spinlock_acquire(&fdt.lock);
	int node = -EINVAL;
	while ((node = fdt_next_node(fdt.fdt, node, nullptr)) >= 0) {
		if (fdt_node_check_compatible(fdt.fdt, node, compat) == 0) {
			spinlock_release(&fdt.lock);
			return node;
		}
	}

	spinlock_release(&fdt.lock);
	return -EINVAL;
}

i32 fdt_traverse_next_node(i32 offset)
{
	spinlock_acquire(&fdt.lock);
	i32 ret = fdt_next_node(fdt.fdt, offset, nullptr);
	spinlock_release(&fdt.lock);
	return ret;
}

const i8 *fdt_get_compat(i32 offset, i32 *len)
{
	spinlock_acquire(&fdt.lock);
	const void *ret = fdt_query_prop_value(offset, "compatible", len);
	spinlock_release(&fdt.lock);
	return ret;
}

bool fdt_get_reg(i32 node, Reg *reg)
{
	spinlock_acquire(&fdt.lock);
	TRACE("get reg = %d", node);
	i32 parent = fdt_parent_offset(fdt.fdt, node);

	i32 len;

	const fdt32_t *ac_prop =
		fdt_getprop(fdt.fdt, parent, "#address-cells", &len);
	const fdt32_t *sc_prop =
		fdt_getprop(fdt.fdt, parent, "#size-cells", &len);

	u32 ac = ac_prop ? fdt32_to_cpu(*ac_prop) : 2;
	u32 sc = sc_prop ? fdt32_to_cpu(*sc_prop) : 1;

	const fdt32_t *reg_ = fdt_query_prop_value(node, "reg", &len);
	if (nullptr == reg_) {
		spinlock_release(&fdt.lock);
		return false;
	}

	u64 address = 0;
	u64 size = 0;

	for (u32 i = 0; i < ac; i++) {
		address = (address << 32) | fdt32_to_cpu(reg_[i]);
	}

	for (u32 i = 0; i < sc; i++) {
		size = (size << 32) | fdt32_to_cpu(reg_[ac + i]);
	}

	reg->address = address;
	reg->size = size;
	spinlock_release(&fdt.lock);
	return true;
}
