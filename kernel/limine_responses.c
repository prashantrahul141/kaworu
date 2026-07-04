#include "limine_responses.h"
#include "common_defs.h"
#include "config.h"
#include "debug/log.h"
#include "limine.h"
#include "types.h"

/* contains all the limine requests' responeses and provides access to them */

/* limine boot protocol revision */
USED SECTION(".limine_requests") static volatile u64 _limine_base_revision[] =
	LIMINE_BASE_REVISION(6);

volatile u64 *limine_base_revision(void)
{
	return _limine_base_revision;
}

/* limine requests */

/* memmap */
USED SECTION(".limine_requests") static volatile struct limine_memmap_request
	_limine_memmap = { .id = LIMINE_MEMMAP_REQUEST_ID, .revision = 0 };

volatile struct limine_memmap_response *limine_memmap(void)
{
	return _limine_memmap.response;
}

/* command line arguments */
USED SECTION(
	".limine_requests") static volatile struct limine_executable_cmdline_request
	cmdline_request = { .id = LIMINE_EXECUTABLE_CMDLINE_REQUEST_ID,
			    .revision = 0 };

/* hhdm */
USED SECTION(".limine_requests") static volatile struct limine_hhdm_request
	_limine_hhdm = { .id = LIMINE_HHDM_REQUEST_ID, .revision = 0 };

volatile struct limine_hhdm_response *limine_hhdm(void)
{
	return _limine_hhdm.response;
}

/* kernel executable address */
USED SECTION(
	".limine_requests") static volatile struct limine_executable_address_request
	_limine_kernel_address = { .id = LIMINE_EXECUTABLE_ADDRESS_REQUEST_ID,
				   .revision = 0 };

volatile struct limine_executable_address_response *limine_kernel_address(void)
{
	return _limine_kernel_address.response;
}

/* framebuffer */
USED SECTION(
	".limine_requests") static volatile struct limine_framebuffer_request
	_limine_framebuffer = { .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
				.revision = 0 };

volatile struct limine_framebuffer_response *limine_framebuffer(void)
{
	return _limine_framebuffer.response;
}

/* limine requests start and end markers */
USED SECTION(".limine_requests_start") static volatile uint64_t
	limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

USED SECTION(".limine_requests_end") static volatile uint64_t
	limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;

void limine_responses_save(void)
{
}
