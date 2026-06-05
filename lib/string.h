#ifndef _STRINGS_H_
#define _STRINGS_H_

#include "types.h"
#include "common_defs.h"

/*
 * Fills first count bytes of the memory pointed by s with byte.
 * returns the original pointer.
 */
void *memset(void *s, u8 byte, size_t count) NONNULL(1);

/*
 * copies size bytes from src to destination.
 * returns original dest pointer.
 */
void *memcpy(void *dest, const void *src, size_t size) NONNULL(1, 2);

/*
 * compares two memory area, returns zero if they are equal, otherwize returns
 * positive if s1 is larger than s2 and vice versa.
 *
 * also returns zero if size = 0.
 */
i32 memcmp(const void *s1, const void *s2, size_t n);

/*
 * Size of string
 */
size_t strlen(const i8 *s);

#endif
