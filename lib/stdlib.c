#include "types.h"
#include "ctype.h"
#include <limits.h>
#include "stdlib.h"

/*
 * Taken from:
 * https://android.googlesource.com/platform/bionic/+/a27d2baa/libc/stdlib/strtoul.c
 * modified to suit in kaworu
 *
 */

usize strtoul(const i8 *nptr, i8 **endptr, i64 base)
{
	const i8 *s;
	u64 acc, cutoff;
	i32 c;
	i32 neg, any;
	/*
	 * See strtol for comments as to the logic used.
	 */
	s = nptr;
	do {
		c = (unsigned char)*s++;
	} while (isspace(c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	} else {
		neg = 0;
		if (c == '+')
			c = *s++;
	}
	if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;
	cutoff = ULONG_MAX / (usize)base;
	i32 cutlim = (i32)(ULONG_MAX % (usize)base);
	for (acc = 0, any = 0;; c = (unsigned char)*s++) {
		if (isdigit(c))
			c -= '0';
		else if (isalpha(c))
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		else
			break;
		if (c >= base)
			break;
		if (any < 0)
			continue;
		if (acc > cutoff || (acc == cutoff && c > cutlim)) {
			any = -1;
			acc = ULONG_MAX;
		} else {
			any = 1;
			acc *= (unsigned long)base;
			acc += (u64)c;
		}
	}
	if (neg && any > 0) {
		acc = -acc;
	}
	if (endptr != nullptr) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-qual"
		// NOLINTNEXTLINE
		*endptr = (i8 *)(any ? s - 1 : nptr);
#pragma clang diagnostic pop
	}
	return (acc);
}
