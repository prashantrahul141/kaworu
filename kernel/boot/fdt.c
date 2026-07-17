#include "boot/fdt.h"
#include "boot/limine_responses.h"
#include "common_defs.h"
#include "debug/assert.h"
#include "debug/panic.h"
#include "error.h"
#include "limine.h"
#include "libfdt.h"
#include "memlayout.h"
#include "mm/kheap.h"

static void *fdt = nullptr;

void fdt_init(void)
{
	INFO("Initializing flat device tree parser");
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
	fdt = kalloc(alloc_size * 3);
	if (IS_ERR(fdt)) {
		panic("out of memory");
	}

	DEBUG("fullsize = %d", total_size);
	memcpy(fdt, dtb_ptr, total_size);
}

static const void *fdt_query_prop_value(i32 node_offset, const i8 *prop,
					i32 *len)
{
	return fdt_getprop(fdt, node_offset, prop, len);
}

bool fdt_get_reg_for_compat(const i8 *compat, Reg *reg)
{
	DEBUG("get reg for compat = %s", compat);
	i32 node = fdt_query_compat(compat);
	if (node < 0) {
		return false;
	}
	return fdt_get_reg(node, reg);
}

i32 fdt_query_compat(const i8 *compat)
{
	DEBUG("query compat  = %s", compat);
	int node = -EINVAL;
	while ((node = fdt_next_node(fdt, node, nullptr)) >= 0) {
		if (fdt_node_check_compatible(fdt, node, compat) == 0) {
			return node;
		}
	}

	return -EINVAL;
}

bool fdt_get_reg(i32 node, Reg *reg)
{
	DEBUG("get reg = %d", node);
	i32 parent = fdt_parent_offset(fdt, node);

	i32 len;

	const fdt32_t *ac_prop =
		fdt_getprop(fdt, parent, "#address-cells", &len);
	const fdt32_t *sc_prop = fdt_getprop(fdt, parent, "#size-cells", &len);

	u32 ac = ac_prop ? fdt32_to_cpu(*ac_prop) : 2;
	u32 sc = sc_prop ? fdt32_to_cpu(*sc_prop) : 1;

	const fdt32_t *reg_ = fdt_query_prop_value(node, "reg", &len);
	if (nullptr == reg_) {
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
	return true;
}
