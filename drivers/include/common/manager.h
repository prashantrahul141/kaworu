#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include "error.h"
#include "io/io.h"
#include "types.h"

typedef struct Device Device;

typedef struct {
	const i8 *name;
	const i8 *const *compatible;
	errno_t (*probe)(Device *);
	errno_t (*remove)(Device *);
} Driver;

typedef struct {
	void (*write)(Device *device, const IOEvent *event);
	u8 (*read)(Device *device);
} DriverOps;

typedef enum {
	DEVICE_UNKNOWN = 0,
	DEVICE_FRAMEBUFFER,
	DEVICE_UART,
} DeviceClass;

typedef struct {
	void (*write)(Device *backend, const IOEvent *event);
	u8 (*read)(Device *backend);
	void (*flush)(Device *backend);
} ConsoleOps;

struct Device {
	const i8 *name;
	const Driver *driver;
	union {
		const ConsoleOps *console_ops;
	};
	DeviceClass class;
	void *driver_data;
	i32 fdt_node_offset;
	Device *next;
};

#define REGISTER_DEVICE_DRIVER(driver) \
	USED SECTION(".driver") const Driver *driver##_ptr = &(driver)

#define ACCESS_DRIVER_DATA(type, device) ((type *)((device)->driver_data))

/*
 * Initialize driver manager
 */
void dmanager_init(void);

/*
 * Registers drivers with found hardware
 */
void dmanager_probe_all(void);

/*
 * Get first device with a specific class
 */
Device *dmanager_get_by_class(DeviceClass class);

#endif // _PLATFORM_H_
