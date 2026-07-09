#include "core/cpu.h"
#include "types.h"
#include "arch/aarch64/aarch64.h"

static Cpu CPUS[CONFIG_CPU_COUNT];

u32 get_cpuid(void)
{
	return r_mpidr_el1() & 0xFF;
}

Cpu *this_cpu(void)
{
	u32 id = get_cpuid();
	return &CPUS[id];
}
