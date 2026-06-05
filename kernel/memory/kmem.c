#include "memory/kmem.h"
#include "printf.h"
#include "spinlock.h"
#include "memlayout.h"
#include "common_defs.h"
#include "string.h"

Kmem kmem = { .free_list = NULL };

static void kmem_free_range(size_t start, size_t end);

void kmem_init(void)
{
	spinlock_init(&kmem.spinlock, "kmem");
	kmem_free_range(FREE_MEM_BOTTOM, FREE_MEM_TOP);
}

void *kmem_alloc(void)
{
	spinlock_acquire(&kmem.spinlock);
	PhyChunk *ret = (PhyChunk *)kmem.free_list;
	if (NULL == ret) {
		// TODO: maybe just return null instead of panicking?
		panic("ran out of memory.");
	}
	kmem.free_list = ret->next;
	spinlock_release(&kmem.spinlock);

	memset(ret, 1, PAGE_SIZE);
	return (void *)ret;
}

void kmem_free(void *py_addr)
{
	if (0 != (size_t)py_addr % PAGE_SIZE) {
		panic("physical address = %p not aligned with PAGE_SIZE = %d",
		      py_addr, PAGE_SIZE);
	}

	if ((size_t)py_addr >= FREE_MEM_TOP) {
		panic("phyical address = %p more than FREE_MEM_TOP = %p",
		      py_addr, FREE_MEM_TOP);
	}

	if ((size_t)py_addr < FREE_MEM_BOTTOM) {
		panic("phyical address = %p less than FREE_MEM_BOTTOM = %p",
		      py_addr, FREE_MEM_BOTTOM);
	}

	// zeroout
	memset(py_addr, 0, PAGE_SIZE);

	PhyChunk *p = (PhyChunk *)py_addr;
	spinlock_acquire(&kmem.spinlock);
	p->next = kmem.free_list;
	kmem.free_list = p;
	spinlock_release(&kmem.spinlock);
}

static void kmem_free_range(size_t start, size_t end)
{
	u8 *p = (u8 *)round_up(start, PAGE_SIZE);
	for (; p + PAGE_SIZE <= (u8 *)end; p += PAGE_SIZE) {
		kmem_free(p);
	}
}
