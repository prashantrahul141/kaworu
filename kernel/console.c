#include "console.h"
#include "common_defs.h"
#include "debug/panic.h"
#include "error.h"
#include "limine.h"
#include "limine_responses.h"
#include "thirdparty/Flanterm/src/flanterm.h"
#include "thirdparty/Flanterm/src/flanterm_backends/fb.h"

/* limine request */

static Console console = { .initialized = false };

/* static function declarations */
static errno_t fb_console_init(void);
static errno_t fb_console_deinit(void);

void console_init(ConsoleDeviceBackendType which_backend)
{
	if (CONSOLE_BACKEND_FRAMEBUFFER == which_backend) {
		if (EOK != fb_console_init()) {
			panic("Failed to initialize frame buffer console");
		}
	}
}

errno_t console_deinit(void)
{
	if (!console.initialized) {
		return EINVAL;
	}

	if (CONSOLE_BACKEND_FRAMEBUFFER == console.backend) {
		return fb_console_deinit();
	}

	console.initialized = false;
	return EOK;
}

errno_t console_write(const i8 *data, usize size)
{
	if (!console.initialized) {
		return EINVAL;
	}

	if (CONSOLE_BACKEND_FRAMEBUFFER == console.backend) {
		flanterm_write(console.backend_ctx, data, size);
	}

	return EOK;
}

errno_t console_write_char(i8 data)
{
	return console_write(&data, 1);
}

errno_t console_set_background(ConsoleColor c)
{
	if (!console.initialized) {
		return EINVAL;
	}

	if (CONSOLE_BACKEND_FRAMEBUFFER == console.backend) {
		flanterm_set_text_bg(console.backend_ctx, c.color, c.bright);
	}

	return EOK;
}

errno_t console_set_foreground(ConsoleColor c)
{
	if (!console.initialized) {
		return EINVAL;
	}

	if (CONSOLE_BACKEND_FRAMEBUFFER == console.backend) {
		flanterm_set_text_fg(console.backend_ctx, c.color, c.bright);
	}

	return EOK;
}

static errno_t fb_console_init(void)
{
	/* are we already initialized? */
	if (console.initialized &&
	    console.backend == CONSOLE_BACKEND_FRAMEBUFFER) {
		return EOK;
	}

	volatile struct limine_framebuffer_response *fb_response =
		limine_framebuffer();

	/* ensure we got a framebuffer */
	if (nullptr == fb_response || fb_response->framebuffer_count < 1) {
		return ENOENT;
	}

	/* Fetch the first framebuffer */
	struct limine_framebuffer *fb = fb_response->framebuffers[0];

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

	flanterm_reset_text_bg(ft_ctx);
	flanterm_reset_text_fg(ft_ctx);

	console.backend_ctx = ft_ctx;
	console.initialized = true;
	console.backend = CONSOLE_BACKEND_FRAMEBUFFER;
	return EOK;
}

static errno_t fb_console_deinit(void)
{
	flanterm_deinit(console.backend_ctx, nullptr);
	console.backend_ctx = nullptr;
	return EOK;
}
