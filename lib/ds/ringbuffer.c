#include "ds/ringbuffer.h"
#include "error.h"
#include <string.h>

errno_t ring_buffer_push(RingBuffer *rb, void *data)
{
	/* next is where head will point to after this write */
	usize next = (rb->head + 1) % rb->capacity;

	/* if the head + 1 == tail, circular buffer is full */
	if (next == rb->tail) {
		return -ENOMEM;
	}

	memcpy(&rb->buffer[rb->head * rb->element_size], data,
	       rb->element_size);
	rb->head = next;
	return EOK;
}

errno_t ring_buffer_pop(RingBuffer *rb, void *data)
{
	/* if the head == tail, we dont have any data */
	if (rb->head == rb->tail) {
		return -ENOMEM;
	}

	memcpy(data, &rb->buffer[rb->tail * rb->element_size],
	       rb->element_size);

	rb->tail = (rb->tail + 1) % rb->capacity;

	return EOK;
}

bool ring_buffer_full(const RingBuffer *rb)
{
	return ((rb->head + 1) % rb->capacity) == rb->tail;
}

bool ring_buffer_empty(const RingBuffer *rb)
{
	return rb->head == rb->tail;
}
