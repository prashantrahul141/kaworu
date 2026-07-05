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

typedef void (*fn_write)(u8 *c, usize len);

typedef enum {
	CONSOLE_BACKEND_FRAMEBUFFER,
	CONSOLE_BACKEND_UART
} ConsoleDeviceBackendType;

typedef struct {
	bool initialized;
	bool is_writable;
	bool is_readable;
	/* data stored by the backend */
	void *backend_ctx;
} Console;

/*
 * initialize given console device
 */
void console_init(ConsoleDeviceBackendType which_backend);

/*
 * deinitialize console device
 */
errno_t console_deinit(void);

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
errno_t console_write(const i8 *data, usize size);
errno_t console_write_char(i8 data);

#endif
