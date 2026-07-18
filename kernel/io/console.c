#include "io/console.h"
#include "debug/log.h"
#include "error.h"
#include "mm/kheap.h"
#include "sync/spinlock.h"

typedef struct {
	SpinLock lock;
	ConsoleBackend *backends;
} Console;

static Console console = {};

errno_t console_init()
{
	spinlock_init(&console.lock, "console");
	return EOK;
}

void console_deinit()
{
}

void console_register_backend(ConsoleBackend *backend)
{
	DEBUG("registering device = %s", backend->device->name);
	spinlock_acquire(&console.lock);
	backend->next = console.backends;
	console.backends = backend;
	spinlock_release(&console.lock);
}

void console_register(Device *device)
{
	ConsoleBackend *backend = kalloc(sizeof(ConsoleBackend));
	backend->device = device;
	console_register_backend(backend);
}

bool console_unregister(const Device *device)
{
	DEBUG("removing device = %s", device->name);
	spinlock_acquire(&console.lock);
	ConsoleBackend *curr = console.backends;
	ConsoleBackend *prev = nullptr;
	while (nullptr != curr) {
		if (curr->device == device) {
			if (prev == nullptr) {
				console.backends = curr->next;
			} else {
				prev->next = curr->next;
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
