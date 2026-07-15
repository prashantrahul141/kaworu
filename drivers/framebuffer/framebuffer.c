#include "framebuffer.h"
#include "boot/limine_responses.h"
#include "debug/log.h"
#include "debug/panic.h"
#include "error.h"
#include "flanterm.h"
#include "flanterm_backends/fb.h"
#include "io/console.h"
#include "limine.h"

typedef struct {
	ConsoleBackend backend;
	struct flanterm_context *flanterm_ctx;
} FramebufferConsoleBackend;

static const ConsoleBackendOps framebuffer_ops = {
	.read = nullptr,
	.write = framebuffer_write_event,
	.flush = framebuffer_flush,
};

static FramebufferConsoleBackend framebuffer_backend = {
	.backend = { .name = "framebuffer",
		     .ops = &framebuffer_ops,
		     .next = nullptr },
	.flanterm_ctx = nullptr
};

static struct flanterm_context *flanterm_init(void);

errno_t framebuffer_init()
{
	DEBUG("setting up flanterm");
	framebuffer_backend.flanterm_ctx = flanterm_init();
	if (IS_ERR(framebuffer_backend.flanterm_ctx)) {
		panic("failed to init framebuffer");
	}
	console_register(&framebuffer_backend.backend);
	return EOK;
}

void framebuffer_write_event(ConsoleBackend *backend, const ConsoleEvent *event)
{
	UNUSED_ARG(backend);
	flanterm_set_text_bg(framebuffer_backend.flanterm_ctx, event->bg.color,
			     event->bg.bright);

	flanterm_set_text_fg(framebuffer_backend.flanterm_ctx, event->fg.color,
			     event->fg.bright);

	flanterm_write(framebuffer_backend.flanterm_ctx, event->msg,
		       event->len);
	flanterm_reset_text_bg(framebuffer_backend.flanterm_ctx);
	flanterm_reset_text_fg(framebuffer_backend.flanterm_ctx);
}

void framebuffer_flush(ConsoleBackend *backend)
{
	UNUSED_ARG(backend);
	flanterm_flush(framebuffer_backend.flanterm_ctx);
}

static struct flanterm_context *flanterm_init(void)
{
	volatile struct limine_framebuffer_response *fb_response =
		limine_framebuffer();

	/* ensure we got a framebuffer */
	if (nullptr == fb_response || fb_response->framebuffer_count < 1) {
		return ERR_TO_PTR(-ENOENT);
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
		return ERR_TO_PTR(-ENOENT);
	}

	flanterm_reset_text_bg(ft_ctx);
	flanterm_reset_text_fg(ft_ctx);
	flanterm_set_autoflush(ft_ctx, false);

	return ft_ctx;
}
