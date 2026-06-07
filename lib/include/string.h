#ifndef _STRINGS_H_
#define _STRINGS_H_

#include "types.h"
#include "common_defs.h"

/*
 * Fills first count bytes of the memory pointed by s with byte.
 * returns the original pointer.
 */
void *memset(void *s, u8 byte, usize count) NONNULL(1);

/*
 * copies size bytes from src to destination.
 * returns original dest pointer.
 */
void *memcpy(void *dest, const void *src, usize size) NONNULL(1, 2);

/*
 * compares two memory area, returns zero if they are equal, otherwize returns
 * positive if s1 is larger than s2 and vice versa.
 *
 * also returns zero if size = 0.
 */
i32 memcmp(const void *s1, const void *s2, usize n);

/*
 * Size of string
 */
usize strlen(const i8 *s);

#endif
