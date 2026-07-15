/*
 * qemu's virt platform provides a PL011 uart
 * PL011 spec: https://developer.arm.com/documentation/ddi0183/g/
 *
 */

#ifndef _UART_H_
#define _UART_H_

#include "io/console.h"
#include "types.h"
#include "memlayout.h"

constexpr usize BAUD_RATE = 115200;
constexpr usize BASE_CLOCK = 24000000;

constexpr usize UARTDR = 0x000; // write register
constexpr usize UARTFR = 0x018; // pooling register
constexpr usize UARTFR_BUSY = (1 << 3); // bit in UARTFR, if transmission is
					// busy
constexpr usize UARTFR_RXFE = (1 << 4); // recieve fifo is empty
constexpr usize UARTIBRD = 0x024; // speed 1
constexpr usize UARTFBRD = 0x028; // speed 2
constexpr usize UARTLCR_H = 0x02C; // line control register
constexpr usize UARTLCR_H_FEN = (1 << 4); // fifo
constexpr usize UARTCR = 0x030; // control register
constexpr usize UARTCR_UARTEN = (1 << 0); // enable/disable uart
constexpr usize UARTCR_TEX = (1 << 8); // recieve enable
constexpr usize UARTCR_REX = (1 << 9); // transmit enable
constexpr usize UARTIMSC = 0x038; // control interrupt
constexpr usize UARTDMACR = 0x048; // control dma

/*
 * Initialize uart
 */
void uart_init(void);

/*
 * Deinitialize uart
 */
void uart_deinit(void);

/*
 * write console event
 */
void uart_write_event(ConsoleBackend *backend, const ConsoleEvent *event);

/*
 * Put a single byte to uart
 */
void uart_putchar(i8 c);

/*
 * Put a c string to uart until null is encountered
 */
void uart_write(const i8 *s);

/*
 * forces writes all pending data
 */
void uart_flush(ConsoleBackend *backend);

/*
 * Put first n characters of string s
 */
void uart_writen(const i8 *s, usize n);

/*
 * Get byte from uart
 */
u8 uart_getchar(void);

#endif // _UART_H_
