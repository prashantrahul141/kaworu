#include "init.h"

void kernel_main(void)
{
	limine_responses_save();
	console_init();
	printf_init();
	printf("booting...\n");
	framebuffer_init();
	kmem_init();
	vm_init();
	kheap_init();
	fdt_init();
	dmanager_init();
	dmanager_probe_all();
	console_register(dmanager_get_by_class(DEVICE_UART), true);

	printf("\nHello from kaworu\n\n");

	printf_deinit();
	console_deinit();
}
