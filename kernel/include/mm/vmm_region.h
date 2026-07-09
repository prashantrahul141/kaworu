/*
 * A very thin wrapper around bitmap allocator for the vmm. it stores allocation
 * sizes
 */

#ifndef _REGION_H_
#define _REGION_H_

#include "allocator/bitmap.h"
#include "memlayout.h"
#include "sync/spinlock.h"

typedef struct {
	void *va;
	usize page_count;
} VMAllocation;
static_assert(sizeof(VMAllocation) == 16, "VMAllocation is not 16 bytes?");

#define MAX_VM_ALLOCATION (PAGE_SIZE / sizeof(VMAllocation))

typedef struct {
	SpinLock lock;
	AllocBitMap allocator;
	VMAllocation allocations[MAX_VM_ALLOCATION];
} VMRegion;

#define STATIC_ALLOC_VM_REGION(name, addr, size)                                \
	static u8 _bitmap_storage_##name[SIZE_TO_BITMAP_BYTES((size))] = { 0 }; \
	static VMRegion name = {                                                \
		.allocator = { .bitmap = _bitmap_storage_##name,                \
			       .page_count = (size) / (PAGE_SIZE),              \
			       .pool = (u8 *)(addr) },                          \
		.allocations = { 0 }                                            \
	};

void region_init(VMRegion *region, const i8 *msg);

VMAllocation *region_find(VMRegion *region, void *addr);

void *region_alloc(VMRegion *region, usize page_count);

usize region_free(VMRegion *region, void *addr);

#endif // _REGION_H_
