#include "allocator/arena.h"
#include "error.h"

void arena_init(Arena *arena, void *pool, usize capacity)
{
	arena->pool = pool;
	arena->capacity = capacity;
}

void *arena_alloc(Arena *arena, usize size)
{
	usize left = arena_left(arena);
	if (left < size) {
		return ERR_TO_PTR(-ENOMEM);
	}

	void *alloc = arena->pool + arena->count;
	arena->count += size;
	return alloc;
}

usize arena_count(const Arena *arena)
{
	return arena->count;
}

usize arena_capacity(const Arena *arena)
{
	return arena->capacity;
}

bool arena_is_full(const Arena *arena)
{
	return arena->count == arena->capacity;
}

bool arena_is_empty(const Arena *arena)
{
	return arena->count == 0;
}

usize arena_left(const Arena *arena)
{
	return arena->capacity - arena->count;
}

bool arena_can_fit(const Arena *arena, usize size)
{
	usize left = arena->capacity - arena->count;
	return left >= size;
}

void arena_reset(Arena *arena)
{
	arena->count = 0;
}

void *arena_base(const Arena *arena)
{
	return arena->pool;
}
