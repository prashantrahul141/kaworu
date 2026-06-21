#include "init.h"
#include "aarch64/aarch64.h"
#include "common_defs.h"
#include "console.h"
#include "config.h"
#include "debug/log.h"
#include "limine.h"
#include "memory/kmem.h"
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
	kmem_init();

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
	hlt(0);
}
