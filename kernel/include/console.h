#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "error.h"
#include "types.h"

typedef void (*fn_write)(u8 *c, usize len);

typedef enum {
	CONSOLE_BACKEND_FRAMEBUFFER,
} ConsoleDeviceBackendType;

typedef struct {
	bool is_writable;
	bool is_readable;
} ConsoleInfo;

typedef struct {
	bool initialized;
	ConsoleInfo info;
	/* write function */
	fn_write write;
	/* data stored by the backend */
	void *backend_ctx;
} Console;

/*
 * initialize given console device
 */
MUST_CHECK errno_t console_init(ConsoleDeviceBackendType which_backend);

/*
 * deinitialize console device
 */
errno_t console_deinit(void);

/*
 * write to console
 */
errno_t console_write(const i8 *data, usize size);
errno_t console_write_char(i8 data);

#endif
