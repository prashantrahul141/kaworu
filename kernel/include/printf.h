#ifndef _PRINT_H_
#define _PRINT_H_

#include "types.h"

void printf_init(void);
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

void printf_deinit(void);

#include "debug/panic.h"

#endif // _PRINT_H_
