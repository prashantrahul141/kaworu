#include "pl011.h"
#include "boot/fdt.h"
#include "debug/panic.h"
#include "error.h"
#include "common/driver_data.h"
#include "mm/kheap.h"
#include "mm/vmm.h"
#include "common/manager.h"
#include "types.h"

constexpr usize BAUD_RATE = 115200;
constexpr usize BASE_CLOCK = 24000000;

constexpr usize UARTDR = 0x000; // write register
constexpr usize UARTFR = 0x018; // pooling register
constexpr usize UARTFR_BUSY = (1 << 3); // bit in UARTFR, if transmission is
					// busy
constexpr usize UARTFR_RXFE = (1 << 4); // recieve fifo is empty
constexpr usize UARTIBRD = 0x024; // speed 1
constexpr usize UARTLCR_H = 0x02C; // line control register
constexpr usize UARTLCR_H_FEN = (1 << 4); // fifo
constexpr usize UARTCR = 0x030; // control register
constexpr usize UARTCR_UARTEN = (1 << 0); // enable/disable uart
constexpr usize UARTCR_TEX = (1 << 8); // recieve enable
constexpr usize UARTCR_REX = (1 << 9); // transmit enable
constexpr usize UARTIMSC = 0x038; // control interrupt
constexpr usize UARTDMACR = 0x048; // control dma

#define reg(base, _reg)		     (volatile u32 *)((_reg) + (base))
#define write_reg(base, _reg, value) (*(reg((base), (_reg))) = (value))
#define read_reg(base, _reg)	     (*(reg((base), (_reg))))

static void wait_tx_complete(const Device *device)
{
	while ((read_reg(ACCESS_DRIVER_DATA(UartDriverData, device)->base_addr,
			 UARTFR) &
		UARTFR_BUSY) != 0)
		;
}

static void wait_rx_ready(const Device *device)
{
	while ((read_reg(ACCESS_DRIVER_DATA(UartDriverData, device)->base_addr,
			 UARTFR) &
		UARTFR_RXFE) != 0)
		;
}

static void uart_putchar(Device *device, i8 c)
{
	wait_tx_complete(device);
	write_reg(ACCESS_DRIVER_DATA(UartDriverData, device)->base_addr, UARTDR,
		  c);
}

static void pl011_write(Device *device, const IOEvent *event)
{
	usize len = 0;
	while (len < event->len) {
		uart_putchar(device, event->msg[len++]);
	}
}

static u8 pl011_read(Device *device)
{
	wait_rx_ready(device);
	return (u8)read_reg(
		ACCESS_DRIVER_DATA(UartDriverData, device)->base_addr, UARTDR);
}

static void pl011_flush(Device *device)
{
	wait_tx_complete(device);
}

static void calculate_divisor(u64 base_clock, u32 baud_rate, u32 *ibrd,
			      u32 *fbrd)
{
	const u32 div = 4 * (u32)base_clock / baud_rate;
	*fbrd = div & 0x3f;
	*ibrd = (div >> 6) & 0xffff;
}

static const ConsoleOps pl011_ops = {
	.write = pl011_write,
	.read = pl011_read,
	.flush = pl011_flush,
};

/*
 * initialize pl011
 */
errno_t pl011_probe(Device *device)
{
	DEBUG("probing pl011");
	Reg reg;
	if (!fdt_get_reg(device->fdt_node_offset, &reg, 1)) {
		return -ENODEV;
	}

	UartDriverData *driver_data = kalloc(sizeof(UartDriverData));
	if (IS_ERR(driver_data)) {
		WARN("failed to allocate mem for uart");
		return -ENOMEM;
	}

	driver_data->base_addr = (usize)vm_mmio_map(reg.address, PAGE_SIZE);
	device->driver_data = driver_data;
	if (IS_ERR((device->driver_data))) {
		WARN("mapping failed");
		kfree(driver_data);
		return -ENOMEM;
	}

	usize base_addr = driver_data->base_addr;

	/* disable uart */
	u32 cr = read_reg(base_addr, UARTCR);
	write_reg(base_addr, UARTCR, (cr & (u32)~UARTCR_UARTEN));

	/* wait for current tranmission to complete */
	wait_tx_complete(device);

	/* flush fifo */
	u32 lcr = read_reg(base_addr, UARTLCR_H) & (u32)~UARTLCR_H_FEN;
	write_reg(base_addr, UARTLCR_H, lcr);

	/* configure uart */

	/* setting baud rate */
	u32 ibrd, fbrd;
	calculate_divisor(BASE_CLOCK, BAUD_RATE, &ibrd, &fbrd);
	write_reg(base_addr, UARTIBRD, ibrd);
	write_reg(base_addr, UARTFR, fbrd);

	/* mask all interrupts */
	/* 0000 0111 1111 1111 */
	write_reg(base_addr, UARTIMSC, 0x7ff);

	/* disable dma */
	write_reg(base_addr, UARTDMACR, 0x0);

	/* set both tx, rx enable and enable uart. */
	cr = read_reg(base_addr, UARTCR);
	write_reg(base_addr, UARTCR,
		  cr | UARTCR_UARTEN | UARTCR_TEX | UARTCR_REX);

	/* populate device operations */
	device->console_ops = &pl011_ops;
	device->name = "uart";
	device->class = DEVICE_UART;

	return EOK;
}

errno_t pl011_remove(Device *device)
{
	DEBUG("removing pl011");
	wait_tx_complete(device);
	vm_mmio_unmap(
		(void *)ACCESS_DRIVER_DATA(UartDriverData, device)->base_addr,
		PAGE_SIZE);
	kfree(device->driver_data);
	return EOK;
}

static const i8 *pl011_compat[] = {
	"arm,pl011",
	nullptr,
};

static const Driver pl011_driver = { .name = "pl011",
				     .probe = pl011_probe,
				     .remove = pl011_remove,
				     .compatible = pl011_compat };

REGISTER_DEVICE_DRIVER(pl011_driver);
