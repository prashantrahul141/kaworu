#include "init.h"

void kernel_main(void)
{
	limine_responses_save();
	console_init();
	printf_init();
	framebuffer_init();
	printf("booting...\n");
	kmem_init();
	vm_init();
	uart_init();

	printf("\nHello from kaworu\n\n");

	printf_deinit();
	console_deinit();
}
