#ifndef _CPU_H_
#define _CPU_H_

#include "ktypes.h"
#include "config.h"

typedef struct {
	u32 cpuid;
	i32 count;
	bool intrd_was_enabled;
} cpu;

u32 get_cpuid(void);
cpu *this_cpu(void);

#endif // _CPU_H_
