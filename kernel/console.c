#include "console.h"
#include "common_defs.h"
#include "error.h"
#include "limine.h"
#include "thirdparty/Flanterm/src/flanterm.h"
#include "thirdparty/Flanterm/src/flanterm_backends/fb.h"

/* limine request */
USED SECTION(
	".limine_requests") static volatile struct limine_framebuffer_request
	framebuffer_request = { .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
				.revision = 0 };

static Console fb_console = { .initialized = false,
			      .info = {
				      .is_writable = true,
				      .is_readable = false,
			      } };

/* static function declarations */
static errno_t fb_console_init(void);
static errno_t fb_console_deinit(void);

MUST_CHECK errno_t console_init(ConsoleDeviceBackendType which_backend)
{
	if (CONSOLE_BACKEND_FRAMEBUFFER == which_backend) {
		return fb_console_init();
	}

	return EINVAL;
}

errno_t console_deinit(void)
{
	errno_t ret = fb_console_deinit();
	if (!ret) {
		return ret;
	}

	return EOK;
}

errno_t console_write(const i8 *data, usize size)
{
	if (fb_console.initialized) {
		flanterm_write(fb_console.backend_ctx, data, size);
	}

	return EOK;
}

errno_t console_write_char(i8 data)
{
	return console_write(&data, 1);
}

static errno_t fb_console_init(void)
{
	/* are we already initialized? */
	if (fb_console.initialized) {
		return EOK;
	}

	/* ensure we got a framebuffer */
	if (nullptr == framebuffer_request.response ||
	    framebuffer_request.response->framebuffer_count < 1) {
		return ENOENT;
	}

	/* Fetch the first framebuffer */
	struct limine_framebuffer *fb =
		framebuffer_request.response->framebuffers[0];

	/* create fb context */
	struct flanterm_context *ft_ctx = flanterm_fb_init(
		nullptr, nullptr, fb->address, fb->width, fb->height, fb->pitch,
		fb->red_mask_size, fb->red_mask_shift, fb->green_mask_size,
		fb->green_mask_shift, fb->blue_mask_size, fb->blue_mask_shift,
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr, 0, 0, 1, 0, 0, 1, FLANTERM_FB_ROTATE_0);

	if (nullptr == ft_ctx) {
		return ENOENT;
	}

	fb_console.backend_ctx = ft_ctx;
	fb_console.initialized = true;

	return EOK;
}

static errno_t fb_console_deinit(void)
{
	if (!fb_console.initialized) {
		return EINVAL;
	}

	flanterm_deinit(fb_console.backend_ctx, nullptr);
	fb_console.initialized = false;
	fb_console.backend_ctx = nullptr;
	return EOK;
}
