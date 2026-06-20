#include "init.h"
#include "aarch64/aarch64.h"
#include "common_defs.h"
#include "memory/kmem.h"
#include "printf.h"
#include "drivers/uart/uart.h"
#include "config.h"
#include "limine.h"

/* limine boot protocol revision */
USED SECTION(".limine_requests") static volatile u64 limine_base_revision[] =
	LIMINE_BASE_REVISION(6);

/* limine requests */

/* command line arguments */
USED SECTION(
	".limine_requests") static volatile struct limine_executable_cmdline_request
	cmdline_request = { .id = LIMINE_EXECUTABLE_CMDLINE_REQUEST_ID,
			    .revision = 0 };

/* frame buffer arguments */
USED SECTION(
	".limine_requests") static volatile struct limine_framebuffer_request
	framebuffer_request = { .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
				.revision = 0 };

/* hhdm */
USED SECTION(".limine_requests") static volatile struct limine_hhdm_request
	hhdm_request = { .id = LIMINE_HHDM_REQUEST_ID, .revision = 0 };

/* limine requests start and end markers */
USED SECTION(".limine_requests_start") static volatile uint64_t
	limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

USED SECTION(".limine_requests_end") static volatile uint64_t
	limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;

void kernel_main(void)
{
	if (false == LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision)) {
		hlt(0);
	}

	/* ensure we got a framebuffer */
	if (framebuffer_request.response == NULL ||
	    framebuffer_request.response->framebuffer_count < 1) {
		hlt(0);
	}

	/* Fetch the first framebuffer */
	struct limine_framebuffer *framebuffer =
		framebuffer_request.response->framebuffers[0];

	/* print a nice pattern to screen as an example */
	/* assumes 32bit rgb */
	volatile u32 *fb_ptr = framebuffer->address;
	for (usize y = 0; y < framebuffer->height; y++) {
		for (usize x = 0; x < framebuffer->width; x++) {
			u32 nX = (u32)(x * 255 / framebuffer->width);
			u32 nY = (u32)(y * 255 / framebuffer->height);
			fb_ptr[y * (framebuffer->pitch / 4) + x] = (nY << 8) |
								   nX;
		}
	}

	if (hhdm_request.response == NULL) {
		hlt(0);
	}

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
