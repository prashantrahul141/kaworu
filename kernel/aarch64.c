#include "aarch64.h"

inline void brk(void)
{
	asm volatile("brk #0");
}
