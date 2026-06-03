#ifndef _PRINT_H_
#define _PRINT_H_

#include "ktypes.h"
#include "aarch64.h"

typedef void (*fn_write)(u8 c);

void printf_init(fn_write write);
/*
 * very dump printf implementation.
 * Supports:
 *   %d -> integers
 *   %f -> floats
 *   %b -> booleans
 *   %p -> address
 *   %x -> integer as hex
 *   %s -> string
 *   %c -> character
 */
void printf(const i8 *fmt, ...);
#define panic(fmt, ...)                               \
	do {                                          \
		printf("PANIC: " fmt, ##__VA_ARGS__); \
		brk();                                \
	} while (0)

#endif // _PRINT_H_
