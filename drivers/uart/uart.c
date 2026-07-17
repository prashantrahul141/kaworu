/*
 * Uart implementation
 */

#include "drivers/uart/uart.h"
#include "boot/fdt.h"
#include "common_defs.h"
#include "debug/panic.h"
#include "mm/vmm.h"
#include "types.h"
#include "io/console.h"

/* static function declarations */
static void pl011_init(void);
static void wait_tx_complete(void);
static void wait_rx_ready(void);
static void calculate_divisor(u64 base_clock, u32 baud_rate, u32 *ibrd,
			      u32 *fbrd);

typedef struct {
	ConsoleBackend backend;
	usize uart_base;
	Reg reg;
} UartConsoleBackend;

static const ConsoleBackendOps uart_ops = {
	.read = nullptr,
	.write = uart_write_event,
	.flush = uart_flush,
};

static UartConsoleBackend uart_backend = {
	.backend = { .name = "uart", .ops = &uart_ops, .next = nullptr },
	.uart_base = 0,
	.reg = { 0 }
};

#define reg(_reg)	       (volatile u32 *)((_reg) + uart_backend.uart_base)
#define write_reg(_reg, value) (*(reg((_reg))) = (value))
#define read_reg(_reg)	       (*(reg((_reg))))

void uart_init(void)
{
	INFO("Initializing uart");
	bool found = fdt_get_reg_for_compat("arm,pl011", &uart_backend.reg);
	if (!found) {
		WARN("uart not found");
		return;
	}

	uart_backend.uart_base =
		(usize)vm_mmio_map(uart_backend.reg.address, PAGE_SIZE);
	if (IS_ERR(((void *)uart_backend.uart_base))) {
		panic("failed to init uart because mapping failed");
	}

	pl011_init();
	console_register(&uart_backend.backend);
}

void uart_deinit(void)
{
	DEBUG("removing uart");
	wait_tx_complete();
	vm_mmio_unmap((void *)uart_backend.uart_base, PAGE_SIZE);
	uart_backend.uart_base = 0;
	console_unregister(&uart_backend.backend);
}

void uart_flush(ConsoleBackend *backend)
{
	UNUSED_ARG(backend);
	wait_tx_complete();
}

void uart_putchar(i8 c)
{
	wait_tx_complete();
	write_reg(UARTDR, c);
}

void uart_write_event(ConsoleBackend *backend, const ConsoleEvent *event)
{
	UNUSED_ARG(backend);
	uart_writen(event->msg, event->len);
}

inline void uart_write(const i8 *s)
{
	while (*s != 0)
		uart_putchar(*s++);
}

inline void uart_writen(const i8 *s, usize n)
{
	while (n-- > 0)
		uart_putchar(*s++);
}

u8 uart_getchar(void)
{
	wait_rx_ready();
	return (u8)read_reg(UARTDR);
}

static void wait_tx_complete(void)
{
	while ((read_reg(UARTFR) & UARTFR_BUSY) != 0)
		;
}

static void wait_rx_ready(void)
{
	while ((read_reg(UARTFR) & UARTFR_RXFE) != 0)
		;
}

static void calculate_divisor(u64 base_clock, u32 baud_rate, u32 *ibrd,
			      u32 *fbrd)
{
	const u32 div = 4 * (u32)base_clock / baud_rate;
	*fbrd = div & 0x3f;
	*ibrd = (div >> 6) & 0xffff;
}

/*
 * initialize pl011
 */
static void pl011_init(void)
{
	/* disable uart */
	u32 cr = read_reg(UARTCR);
	write_reg(UARTCR, (cr & (u32)~UARTCR_UARTEN));

	/* wait for current tranmission to complete */
	wait_tx_complete();

	/* flush fifo */
	u32 lcr = read_reg(UARTLCR_H) & (u32)~UARTLCR_H_FEN;
	write_reg(UARTLCR_H, lcr);

	/* configure uart */

	/* setting baud rate */
	u32 ibrd, fbrd;
	calculate_divisor(BASE_CLOCK, BAUD_RATE, &ibrd, &fbrd);
	write_reg(UARTIBRD, ibrd);
	write_reg(UARTFR, fbrd);

	/* mask all interrupts */
	/* 0000 0111 1111 1111 */
	write_reg(UARTIMSC, 0x7ff);

	/* disable dma */
	write_reg(UARTDMACR, 0x0);

	/* set both tx, rx enable and enable uart. */
	cr = read_reg(UARTCR);
	write_reg(UARTCR, cr | UARTCR_UARTEN | UARTCR_TEX | UARTCR_REX);
}
