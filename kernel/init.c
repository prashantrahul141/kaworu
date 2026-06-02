#include "init.h"
#include "printf.h"
#include "uart.h"
#include "aarch64.h"

void kernel_main(void)
{
	uart_init();
	printf_init(uart_putchar);
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
	brk();
}
