#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "error.h"
#include "manager/manager.h"
#include "types.h"
#include "io.h"

typedef struct ConsoleBackend ConsoleBackend;

struct ConsoleBackend {
	Device *device;
	ConsoleBackend *next;
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
void console_register(Device *backend);

/* register a consolebackend directly */
void console_register_backend(ConsoleBackend *backend);

/*
 * unregister a device from console
 */
bool console_unregister(const Device *backend);

/*
 * set foreground color if the underlying backend supports it.
 */
errno_t console_set_foreground(const IOColor c);

/*
 * set background color if the underlying backend supports it.
 */
errno_t console_set_background(const IOColor c);

/*
 * write to console
 */
errno_t console_write(const IOEvent event);

/*
 * flush all backend
 */
errno_t console_flush();

/*
 * write a single char with default properties
 */
errno_t console_write_char(const i8 c);

#endif
