#include "init.h"

void kernel_main(void)
{
	if (false == LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision())) {
		hlt(0);
	}

	limine_responses_save();

	console_init(CONSOLE_BACKEND_FRAMEBUFFER);
	printf_init();
	printf("booting...\n");
	kmem_init();
	vm_mem_init();
	console_init(CONSOLE_BACKEND_UART);

	printf("\nHello from kaworu\n\n");

	printf_deinit();
	console_deinit();
}
