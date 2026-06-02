#ifndef _PRINT_H_
#define _PRINT_H_

#include "ktypes.h"

typedef void (*fn_write)(u8 c);

void printf_init(fn_write write);
void printf(const i8 *fmt, ...);

#endif // _PRINT_H_
