#ifndef _PRINT_H_
#define _PRINT_H_

#include "types.h"
#include <stdarg.h>

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

void vprintf(const i8 *fmt, va_list arg);

void printf_deinit(void);

#endif // _PRINT_H_
