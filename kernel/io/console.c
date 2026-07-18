#include "io/console.h"
#include "debug/log.h"
#include "error.h"
#include "io/io.h"
#include "mm/kheap.h"
#include "sync/spinlock.h"

typedef struct {
	SpinLock lock;
	ConsoleBackend *backends;
	ConsoleBackend *primary;
} Console;

static Console console = {
	.backends = nullptr,
	.primary = nullptr,
};

errno_t console_init()
{
	spinlock_init(&console.lock, "console");
	return EOK;
}

void console_deinit()
{
}

void console_register_backend(ConsoleBackend *backend, bool set_default)
{
	DEBUG("registering device = %s setting default = %b",
	      backend->device->name, set_default);
	spinlock_acquire(&console.lock);
	backend->next = console.backends;
	console.backends = backend;
	if (set_default) {
		console.primary = backend;
	}
	spinlock_release(&console.lock);
}

void console_register(Device *device, bool set_default)
{
	ConsoleBackend *backend = kalloc(sizeof(ConsoleBackend));
	backend->device = device;
	console_register_backend(backend, set_default);
}

bool console_unregister(const Device *device)
{
	DEBUG("removing device = %s", device->name);
	spinlock_acquire(&console.lock);
	ConsoleBackend *curr = console.backends;
	ConsoleBackend *prev = nullptr;
	while (nullptr != curr) {
		/* found backend */
		if (curr->device == device) {
			/* if this was set as default, remove it */
			if (prev == nullptr) {
				console.backends = curr->next;
			} else {
				prev->next = curr->next;
			}

			if (console.primary != nullptr &&
			    console.primary->device == device) {
				console.primary = nullptr;
			}

			curr->next = nullptr;
			spinlock_release(&console.lock);
			return true;
		}

		prev = curr;
		curr = curr->next;
	}

	spinlock_release(&console.lock);
	return false;
}

static inline void write_to_all_backends(IOEvent *ev)
{
	ConsoleBackend *backend = console.backends;
	while (nullptr != backend) {
		backend->device->console_ops->write(backend->device, ev);
		backend = backend->next;
	}
}

errno_t console_write(IOEvent e)
{
	write_to_all_backends(&e);
	console_flush();
	return EOK;
}

errno_t console_flush()
{
	ConsoleBackend *backend = console.backends;
	while (nullptr != backend) {
		backend->device->console_ops->flush(backend->device);
		backend = backend->next;
	}
	return EOK;
}

errno_t console_write_char(i8 c)
{
	IOEvent e = { .msg = &c,
		      .len = 1,
		      .bg = IO_DEFAULT_COLOR_BG,
		      .fg = IO_DEFAULT_COLOR_FG };
	return console_write(e);
}

bool console_read(u8 *out)
{
	if (nullptr == console.backends) {
		return false;
	}

	*out = console.primary->device->console_ops->read(
		console.primary->device);
	return true;
}
