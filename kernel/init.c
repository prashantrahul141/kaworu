#include "init.h"
#include "uart.h"
#include "aarch64.h"

void kernel_main(void)
{
	uart_init();
	uart_print((const u8 *)"Hello, World!\n");
	u8 c = uart_getchar();
	uart_print((const u8 *)"Typed: ");
	uart_putchar(c);
	uart_putchar('\n');
	brk();
}
