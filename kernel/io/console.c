#include "io/console.h"
#include "error.h"
#include "sync/spinlock.h"

#define MAX_CONSOLE_BUFFER_SIZE 10

typedef struct {
	SpinLock lock;
	ConsoleBackend *backends;
} Console;

static Console console = { .backends = nullptr };

errno_t console_init()
{
	spinlock_init(&console.lock, "console");
	return EOK;
}

void console_deinit()
{
}

void console_register(ConsoleBackend *backend)
{
	spinlock_acquire(&console.lock);
	backend->next = console.backends;
	console.backends = backend;
	spinlock_release(&console.lock);
}

bool console_unregister(ConsoleBackend *backend)
{
	spinlock_acquire(&console.lock);
	ConsoleBackend **curr = &console.backends;
	while (nullptr != *curr) {
		if (*curr == backend) {
			*curr = backend->next;
			backend->next = nullptr;
			spinlock_release(&console.lock);
			return true;
		}

		curr = &(*curr)->next;
	}

	spinlock_release(&console.lock);
	return false;
}

static inline void write_to_all_backends(ConsoleEvent *ev)
{
	ConsoleBackend *backend = console.backends;
	while (nullptr != backend) {
		backend->ops->write(backend, ev);
		backend = backend->next;
	}
}

errno_t console_write(ConsoleEvent e)
{
	spinlock_acquire(&console.lock);
	write_to_all_backends(&e);
	console_flush();
	spinlock_release(&console.lock);
	return EOK;
}

errno_t console_flush()
{
	ConsoleBackend *backend = console.backends;
	while (nullptr != backend) {
		backend->ops->flush(backend);
		backend = backend->next;
	}
	return EOK;
}

errno_t console_write_char(i8 c)

{
	ConsoleEvent e = { .msg = &c,
			   .len = 1,
			   .bg = CONSOLE_DEFAULT_COLOR_BG,
			   .fg = CONSOLE_DEFAULT_COLOR_FG };
	return console_write(e);
}
