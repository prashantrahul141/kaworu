#include "init.h"
#include "common_defs.h"
#include "memory/kmem.h"
#include "printf.h"
#include "drivers/uart/uart.h"
#include "config.h"

/*
 * Each cpu gets its own stack depending on their cpuid (0,1,2,3)
 * sp = stack0 + ((cpuid + 1) * STACK_SIZE)
 */
ALIGNED(16)
SECTION(".bss")
u8 stack0[CONFIG_PER_CPU_STACK_SIZE * CONFIG_CPU_COUNT];

void kernel_main(void)
{
	uart_init();
	printf_init(uart_putchar);
	kmem_init();
	for (;;) {
		printf("\nHello from %s\n", "kaworu");
		printf("character c = %c\n", 'c');
		printf("character A as ascii = %d\n", 'A');
		printf("10 = %d\n", 10);
		printf("17 in hex is %x\n", 35);
		printf("203 in hex is %p\n", 203);
		printf("1.0 = %f\n", 1.0);
		printf("-0.0 = %f\n", -0.0);
		printf("2332323.23232323 = %f\n", 2332323.23232323);
		printf("1.99999999999 = %f\n", 1.99999999999);
		printf("-1.99999999999 = %f\n", -1.99999999999);
		printf("-1.123456 = %f\n", -1.123456);
	}
}
