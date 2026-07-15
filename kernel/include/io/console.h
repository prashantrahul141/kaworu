#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "error.h"
#include "types.h"

enum {
	_CONSOLE_COLOR_BLACK = 0,
	_CONSOLE_COLOR_RED = 1,
	_CONSOLE_COLOR_GREEN = 2,
	_CONSOLE_COLOR_YELLOW = 3,
	_CONSOLE_COLOR_BLUE = 4,
	_CONSOLE_COLOR_MAGENTA = 5,
	_CONSOLE_COLOR_CYAN = 6,
	_CONSOLE_COLOR_WHITE = 7,
};

typedef struct {
	u8 color;
	bool bright;
} ConsoleColor;

constexpr ConsoleColor CONSOLE_COLOR_BLACK = { .color = _CONSOLE_COLOR_BLACK,
					       .bright = false };
constexpr ConsoleColor CONSOLE_COLOR_BLACK_BRIGHT = {
	.color = _CONSOLE_COLOR_BLACK,
	.bright = true
};

constexpr ConsoleColor CONSOLE_COLOR_RED = { .color = _CONSOLE_COLOR_RED,
					     .bright = false };
constexpr ConsoleColor CONSOLE_COLOR_RED_BRIGHT = { .color = _CONSOLE_COLOR_RED,
						    .bright = true };

constexpr ConsoleColor CONSOLE_COLOR_GREEN = { .color = _CONSOLE_COLOR_GREEN,
					       .bright = false };
constexpr ConsoleColor CONSOLE_COLOR_GREEN_BRIGHT = {
	.color = _CONSOLE_COLOR_GREEN,
	.bright = true
};

constexpr ConsoleColor CONSOLE_COLOR_YELLOW = { .color = _CONSOLE_COLOR_YELLOW,
						.bright = false };
constexpr ConsoleColor CONSOLE_COLOR_YELLOW_BRIGHT = {
	.color = _CONSOLE_COLOR_YELLOW,
	.bright = true
};

constexpr ConsoleColor CONSOLE_COLOR_BLUE = { .color = _CONSOLE_COLOR_BLUE,
					      .bright = false };
constexpr ConsoleColor CONSOLE_COLOR_BLUE_BRIGHT = {
	.color = _CONSOLE_COLOR_BLUE,
	.bright = true
};

constexpr ConsoleColor CONSOLE_COLOR_MAGENTA = { .color =
							 _CONSOLE_COLOR_MAGENTA,
						 .bright = false };
constexpr ConsoleColor CONSOLE_COLOR_MAGENTA_BRIGHT = {
	.color = _CONSOLE_COLOR_MAGENTA,
	.bright = true
};

constexpr ConsoleColor CONSOLE_COLOR_CYAN = { .color = _CONSOLE_COLOR_CYAN,
					      .bright = false };
constexpr ConsoleColor CONSOLE_COLOR_CYAN_BRIGHT = {
	.color = _CONSOLE_COLOR_CYAN,
	.bright = true
};

constexpr ConsoleColor CONSOLE_COLOR_WHITE = { .color = _CONSOLE_COLOR_WHITE,
					       .bright = false };
constexpr ConsoleColor CONSOLE_COLOR_WHITE_BRIGHT = {
	.color = _CONSOLE_COLOR_WHITE,
	.bright = true
};

constexpr ConsoleColor CONSOLE_DEFAULT_COLOR_FG = CONSOLE_COLOR_WHITE_BRIGHT;
constexpr ConsoleColor CONSOLE_DEFAULT_COLOR_BG = CONSOLE_COLOR_BLACK;

typedef enum {
	CONSOLE_BACKEND_FRAMEBUFFER,
	CONSOLE_BACKEND_UART
} ConsoleDeviceBackendType;

typedef struct {
	ConsoleColor fg;
	ConsoleColor bg;
	const i8 *msg;
	usize len;
} ConsoleEvent;

typedef struct ConsoleBackendOps ConsoleBackendOps;

typedef struct ConsoleBackend ConsoleBackend;

struct ConsoleBackend {
	const i8 *name;
	const ConsoleBackendOps *ops;
	ConsoleBackend *next;
};

struct ConsoleBackendOps {
	void (*write)(ConsoleBackend *backend, const ConsoleEvent *event);
	u8 (*read)(ConsoleBackend *backend);
	void (*flush)(ConsoleBackend *backend);
};

/*
 * initialize given console device
 */
errno_t console_init();

/*
 * deinitialize a console device
 */
void console_deinit();

/*
 * register a device to console
 */
void console_register(ConsoleBackend *backend);

/*
 * unregister a device from console
 */
bool console_unregister(ConsoleBackend *backend);

/*
 * set foreground color if the underlying backend supports it.
 */
errno_t console_set_foreground(ConsoleColor c);

/*
 * set background color if the underlying backend supports it.
 */
errno_t console_set_background(ConsoleColor c);

/*
 * write to console
 */
errno_t console_write(ConsoleEvent event);

/*
 * flush all backend
 */
errno_t console_flush();

/*
 * write a single char with default properties
 */
errno_t console_write_char(i8 c);

#endif
