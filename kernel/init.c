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

	DEBUG("Hello from %s", "kaworu");
	INFO("character c = %c", 'c');
	WARN("character A as ascii = %d", 'A');
	ERROR("10 = %d", 10);
	FATAL("17 in hex is %x", 35);
	DEBUG("203 in hex is %p", 203);
	INFO("1.0 = %f", 1.0);
	WARN("-0.0 = %f", -0.0);
	ERROR("2332323.23232323 = %f", 2332323.23232323);
	FATAL("1.99999999999 = %f", 1.99999999999);
	DEBUG("-1.99999999999 = %f", -1.99999999999);
	INFO("-1.123456 = %f", -1.123456);

	printf_deinit();
	console_deinit();
}
