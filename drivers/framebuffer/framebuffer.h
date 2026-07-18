#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include "io/console.h"

typedef struct {
	ConsoleBackend backend;
	usize uart_base;
} UartConsoleBackend;

errno_t framebuffer_init();
errno_t framebuffer_deinit();

void framebuffer_write_event(Device *backend, const IOEvent *event);

void framebuffer_flush(Device *backend);

#endif // _FRAMEBUFFER_H_
