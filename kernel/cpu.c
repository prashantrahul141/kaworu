#include "cpu.h"
#include "types.h"
#include "aarch64.h"

static cpu CPUS[CONFIG_CPU_COUNT];

u32 get_cpuid(void)
{
	return r_mpidr_el1() & 0xFF;
}

cpu *this_cpu(void)
{
	u32 id = get_cpuid();
	return &CPUS[id];
}
