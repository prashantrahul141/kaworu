#include "init.h"
#include "aarch64/aarch64.h"
#include "common_defs.h"
#include "console.h"
#include "config.h"
#include "limine.h"
#include "printf.h"

/* limine boot protocol revision */
USED SECTION(".limine_requests") static volatile u64 limine_base_revision[] =
	LIMINE_BASE_REVISION(6);

/* limine requests */

/* command line arguments */
USED SECTION(
	".limine_requests") static volatile struct limine_executable_cmdline_request
	cmdline_request = { .id = LIMINE_EXECUTABLE_CMDLINE_REQUEST_ID,
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

	console_init(CONSOLE_BACKEND_FRAMEBUFFER);
	printf_init();

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

	printf_deinit();
	console_deinit();
	hlt(0);
}
