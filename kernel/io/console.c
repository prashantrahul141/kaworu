#include "io/console.h"
#include "common_defs.h"
#include "debug/panic.h"
#include "error.h"
#include "limine.h"
#include "boot/limine_responses.h"
#include "thirdparty/Flanterm/src/flanterm.h"
#include "thirdparty/Flanterm/src/flanterm_backends/fb.h"
#include "uart/uart.h"

static Console fb_console = {
	.initialized = false,
	.is_writable = true,
	.is_readable = false,
};
static Console uart_console = { .initialized = false,
				.is_writable = true,
				.is_readable = true };

/* static function declarations */
static errno_t fb_console_init(void);
static errno_t fb_console_deinit(void);
static errno_t uart_console_init(void);
static errno_t uart_console_deinit(void);

void console_init(ConsoleDeviceBackendType which_backend)
{
	switch (which_backend) {
	case CONSOLE_BACKEND_FRAMEBUFFER: {
		if (EOK != fb_console_init()) {
			panic("Failed to initialize frame buffer console");
		}
		break;
	}

	case CONSOLE_BACKEND_UART: {
		if (EOK != uart_console_init()) {
			panic("Failed to initialize uart console");
		}
		break;
	}

	default: {
		panic("unknown console backend");
	}
	}
}

errno_t console_deinit(ConsoleDeviceBackendType which_backend)
{
	switch (which_backend) {
	case CONSOLE_BACKEND_FRAMEBUFFER: {
		if (fb_console.initialized) {
			fb_console_deinit();
			fb_console.initialized = false;
		}
		break;
	}
	case CONSOLE_BACKEND_UART: {
		if (uart_console.initialized) {
			uart_console_deinit();
			uart_console.initialized = false;
		}
	} break;
	default: {
		panic("corrupted backend");
	}
	}

	return EOK;
}

errno_t console_deinit_all(void)
{
	console_deinit(CONSOLE_BACKEND_FRAMEBUFFER);
	console_deinit(CONSOLE_BACKEND_UART);
	return EOK;
}

errno_t console_write(const i8 *data, usize size)
{
	if (fb_console.initialized) {
		flanterm_write(fb_console.backend_ctx, data, size);
	}

	if (uart_console.initialized) {
		uart_printn(data, size);
	}

	return EOK;
}

errno_t console_write_char(i8 data)
{
	return console_write(&data, 1);
}

errno_t console_set_background(ConsoleColor c)
{
	if (fb_console.initialized) {
		flanterm_set_text_bg(fb_console.backend_ctx, c.color, c.bright);
	}

	return EOK;
}

errno_t console_set_foreground(ConsoleColor c)
{
	if (fb_console.initialized) {
		flanterm_set_text_fg(fb_console.backend_ctx, c.color, c.bright);
	}

	return EOK;
}

static errno_t fb_console_init(void)
{
	/* are we already initialized? */
	if (fb_console.initialized) {
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

	fb_console.backend_ctx = ft_ctx;
	fb_console.initialized = true;
	return EOK;
}

static errno_t fb_console_deinit(void)
{
	flanterm_deinit(fb_console.backend_ctx, nullptr);
	fb_console.backend_ctx = nullptr;
	return EOK;
}

static errno_t uart_console_init(void)
{
	uart_init();
	uart_console.initialized = true;
	return EOK;
}

static errno_t uart_console_deinit(void)
{
	uart_deinit();
	return EOK;
}
