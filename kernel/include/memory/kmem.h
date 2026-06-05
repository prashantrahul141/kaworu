/*
 * Physical memory allocator for user and kernel space.
 */

#ifndef _KMEM_H_
#define _KMEM_H_

#include "aarch64/aarch64.h"
#include "spinlock.h"

typedef struct PhyChunk PhyChunk;

struct PhyChunk {
	PhyChunk *next;
};

typedef struct {
	SpinLock spinlock;
	PhyChunk *free_list;
} Kmem;

/*
 * Init kernel physical memory allocator.
 */
void kmem_init(void);

/* Get a physical memory allocation of PAGE_SIZE size
 */
void *kmem_alloc(void);

/*
 * Return back an allocation
 */
void kmem_free(void *ph_addr);

#endif // _KMEM_H_
