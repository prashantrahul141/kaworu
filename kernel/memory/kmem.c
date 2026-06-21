#include "memory/kmem.h"
#include "debug/assert.h"
#include "error.h"
#include "limine.h"
#include "debug/panic.h"
#include "spinlock.h"
#include "memlayout.h"
#include "common_defs.h"
#include "string.h"

USED SECTION(".limine_requests") static volatile struct limine_memmap_request
	memory_map_request = { .id = LIMINE_MEMMAP_REQUEST_ID, .revision = 0 };

USED SECTION(".limine_requests") static volatile struct limine_hhdm_request
	hhdm_request = { .id = LIMINE_HHDM_REQUEST_ID, .revision = 0 };

Kmem kmem = { .free_list = nullptr };

static void kmem_free_range(usize start, usize end);

void kmem_init(void)
{
	if (nullptr == memory_map_request.response) {
		panic("memory was not mapped by limine");
	}

	if (nullptr == hhdm_request.response) {
		panic("hhdm failed?");
	}

	spinlock_init(&kmem.spinlock, "kmem");

	DEBUG("creating freelist");
	UNUSED usize count = 1;
	for (usize index = 0; index < memory_map_request.response->entry_count;
	     index++) {
		struct limine_memmap_entry *entry =
			memory_map_request.response->entries[index];
		switch (entry->type) {
		case LIMINE_MEMMAP_USABLE: {
			DEBUG("entry [%d] base = %p, length = %p, end = "
			      "%p",
			      count, entry->base, entry->length,
			      entry->base + entry->length);
			kmem_free_range(entry->base,
					entry->base + entry->length);
			count++;
			break;
		}
		default:
			break;
		}
	}
}

void *kmem_alloc(void)
{
	spinlock_acquire(&kmem.spinlock);
	PhyChunk *ret = kmem.free_list;
	if (nullptr == ret) {
		spinlock_release(&kmem.spinlock);
		return ERR_TO_PTR(-ENOMEM);
	}
	kmem.free_list = ret->next;
	spinlock_release(&kmem.spinlock);

	memset(ret, 1, PAGE_SIZE);
	return (void *)ret;
}

void kmem_free(void *py_addr)
{
	TRACE("freeing addr = %p", py_addr);
	if (0 != (usize)py_addr % PAGE_SIZE) {
		panic("physical address = %p not aligned with PAGE_SIZE = %d",
		      py_addr, PAGE_SIZE);
	}

	// zeroout
	memset(py_addr, 0, PAGE_SIZE);

	PhyChunk *p = py_addr;
	spinlock_acquire(&kmem.spinlock);
	p->next = kmem.free_list;
	kmem.free_list = p;
	spinlock_release(&kmem.spinlock);
	TRACE("freed addr = %p", py_addr);
}

/*
 * converts physical address to virtual address
 */
void *phys_to_virt(usize phy)
{
	return (void *)(phy + hhdm_request.response->offset);
}

/*
 * converts virtual address to physical address
 */
usize virt_to_phys(const void *virt)
{
	usize v = (usize)virt;
	ASSERT(v >= hhdm_request.response->offset, "virtual address is larger "
						   "than hhdm offset");
	return v - hhdm_request.response->offset;
}

static void kmem_free_range(usize start, usize end)
{
	u8 *p = (u8 *)start;
	usize free_count = 0;
	for (; p + PAGE_SIZE <= (u8 *)end; p += PAGE_SIZE) {
		u8 *v = phys_to_virt((usize)p);
		kmem_free(v);
		free_count += 1;
	}
	DEBUG("freed %d pages", free_count);
}
