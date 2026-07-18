#include "manager/manager.h"
#include "boot/fdt.h"
#include "debug/log.h"
#include "error.h"
#include "memlayout.h"
#include "config.h"
#include "string.h"
#include "mm/kheap.h"
#include "sync/spinlock.h"

typedef struct {
	Device *device_list;
	SpinLock lock;
} DriverManager;

static DriverManager dmanager = {};

static Driver *find_compat_driver(const i8 *compat);
static void insert_device(Device *device);

/*
 * Initialize driver manager
 */
void dmanager_init(void)
{
	INFO("Initializing driver manager");
	spinlock_init(&dmanager.lock, "driver manager");
}

/*
 * Registers drivers with found hardware
 */
void dmanager_probe_all(void)
{
	INFO("Probing devices");
	spinlock_acquire(&dmanager.lock);
	/* for each fdt node */
	for (i32 offset = fdt_traverse_next_node(-1); offset >= 0;
	     offset = fdt_traverse_next_node(offset)) {
		TRACE("traverse offset = %d", offset);

		/* get compatiblity for this fdt node */
		i32 len;
		const i8 *compat = fdt_get_compat(offset, &len);
		if (nullptr == compat) {
			continue;
		}

		/* find compatible driver for this node */
		Driver *driver = find_compat_driver(compat);
		if (IS_ERR(driver)) {
			continue;
		}
		DEBUG("trying to initialize driver = %s", driver->name);

		/* create and prepopulate device */
		Device *device = kalloc(sizeof(Device));
		device->fdt_node_offset = offset;
		device->driver = driver;

		/* probe driver */
		errno_t ret = device->driver->probe(device);
		if (EOK != ret) {
			WARN("failed probing driver = %s", driver->name);
			kfree(device);
			continue;
		}

		INFO("Discovered device = %s with driver = %s", device->name,
		     driver->name);
		insert_device(device);
	}

	spinlock_release(&dmanager.lock);
}

Device *dmanager_get_by_class(DeviceClass class)
{
	DEBUG("get by class = %d", class);
	spinlock_acquire(&dmanager.lock);

	Device *curr = dmanager.device_list;
	while (nullptr != curr) {
		if (curr->class == class) {
			spinlock_release(&dmanager.lock);
			return curr;
		}
		curr = curr->next;
	}

	spinlock_release(&dmanager.lock);
	return ERR_TO_PTR(-ENOENT);
}

static Driver *find_compat_driver(const i8 *req_compat)
{
	TRACE("finding compat driver, required = %s", req_compat);
	/* for each driver */
	for (Driver **_driver = (Driver **)__KERNEL_TEXT_DRIVERS_START;
	     _driver < (Driver **)__KERNEL_TEXT_DRIVERS_END; _driver++) {
		Driver *driver = *(_driver);

		/* match against each compat of this driver */
		const i8 *const *compat = driver->compatible;
		while (nullptr != *compat) {
			/* if found matching, return */
			if (0 == strcmp(*compat, req_compat)) {
				return driver;
			}
			compat++;
		}
	}
	return ERR_TO_PTR(-ENOENT);
}

static void insert_device(Device *device)
{
	device->next = dmanager.device_list;
	dmanager.device_list = device;
}
