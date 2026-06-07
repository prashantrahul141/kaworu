#include "string.h"

void *memset(void *s, u8 byte, usize count)
{
	u8 *ptr = s;
	while (count-- > 0)
		*ptr++ = byte;
	return s;
}

void *memcpy(void *dest, const void *src, usize size)
{
	u8 *dest_ptr = dest;
	const u8 *src_ptr = src;
	while (size-- > 0)
		*dest_ptr++ = *src_ptr++;
	return dest;
}

i32 memcmp(const void *s1, const void *s2, usize n)
{
	const i8 *p1 = (const i8 *)s1;
	const i8 *p2 = (const i8 *)s2;
	while (n-- > 0) {
		return *p1++ > *p2++ ? 1 : -1;
	}
	return 0;
}

usize strlen(const i8 *s)
{
	const i8 *p = s;
	while (0 != *p++)
		;
	return (usize)(p - s - 1);
}
