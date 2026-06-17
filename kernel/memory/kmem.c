#include "memory/kmem.h"
#include "error.h"
#include "printf.h"
#include "spinlock.h"
#include "memlayout.h"
#include "common_defs.h"
#include "string.h"

Kmem kmem = { .free_list = nullptr };

static void kmem_free_range(usize start, usize end);

void kmem_init(void)
{
	spinlock_init(&kmem.spinlock, "kmem");
	kmem_free_range(FREE_MEM_BOTTOM, FREE_MEM_TOP);
}

void *kmem_alloc(void)
{
	spinlock_acquire(&kmem.spinlock);
	PhyChunk *ret = kmem.free_list;
	if (nullptr == ret) {
		return ERR_TO_PTR(-ENOMEM);
	}
	kmem.free_list = ret->next;
	spinlock_release(&kmem.spinlock);

	memset(ret, 1, PAGE_SIZE);
	return (void *)ret;
}

void kmem_free(void *py_addr)
{
	if (0 != (usize)py_addr % PAGE_SIZE) {
		panic("physical address = %p not aligned with PAGE_SIZE = %d",
		      py_addr, PAGE_SIZE);
	}

	if ((usize)py_addr >= FREE_MEM_TOP) {
		panic("phyical address = %p more than FREE_MEM_TOP = %p",
		      py_addr, FREE_MEM_TOP);
	}

	if ((usize)py_addr < FREE_MEM_BOTTOM) {
		panic("phyical address = %p less than FREE_MEM_BOTTOM = %p",
		      py_addr, FREE_MEM_BOTTOM);
	}

	// zeroout
	memset(py_addr, 0, PAGE_SIZE);

	PhyChunk *p = py_addr;
	spinlock_acquire(&kmem.spinlock);
	p->next = kmem.free_list;
	kmem.free_list = p;
	spinlock_release(&kmem.spinlock);
}

static void kmem_free_range(usize start, usize end)
{
	u8 *p = (u8 *)round_up(start, PAGE_SIZE);
	for (; p + PAGE_SIZE <= (u8 *)end; p += PAGE_SIZE) {
		kmem_free(p);
	}
}
