#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

#include "error.h"
#include "types.h"

typedef struct {
	u8 *buffer;
	usize capacity;
	usize element_size;
	usize head;
	usize tail;
} RingBuffer;

#define STATIC_ALLOC_RING_BUFFER(name, T, _capacity)                           \
	static T _ringbuffer_storage_##name[((sizeof(T) + 1) *                 \
					     (_capacity))] = { 0 };            \
	static RingBuffer name = { .buffer = (u8 *)_ringbuffer_storage_##name, \
				   .capacity = (_capacity),                    \
				   .element_size = sizeof(T),                  \
				   .head = 0,                                  \
				   .tail = 0 };

/*
 * push and pop for ring buffer.
 */
errno_t ring_buffer_push(RingBuffer *rb, void *data);
errno_t ring_buffer_pop(RingBuffer *rb, void *data);

bool ring_buffer_full(const RingBuffer *rb);

bool ring_buffer_empty(const RingBuffer *rb);

#endif // _RING_BUFFER_H_
