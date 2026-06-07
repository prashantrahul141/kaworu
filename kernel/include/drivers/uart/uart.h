/*
 * qemu's virt platform provides a PL011 uart
 * PL011 spec: https://developer.arm.com/documentation/ddi0183/g/
 *
 */

#ifndef _UART_H_
#define _UART_H_

#include "types.h"
#include "memlayout.h"

#define BASE_CLOCK 24000000
#define BAUD_RATE  115200

#define UARTDR	      0x000 // write register
#define UARTFR	      0x018 // pooling register
#define UARTFR_BUSY   (1 << 3) // bit in UARTFR, if transmission is busy
#define UARTFR_RXFE   (1 << 4) // recieve fifo is empty
#define UARTIBRD      0x024 // speed 1
#define UARTFBRD      0x028 // speed 2
#define UARTLCR_H     0x02C // line control register
#define UARTLCR_H_FEN (1 << 4) // fifo
#define UARTCR	      0x030 // control register
#define UARTCR_UARTEN (1 << 0) // enable/disable uart
#define UARTCR_TEX    (1 << 8) // recieve enable
#define UARTCR_REX    (1 << 9) // transmit enable
#define UARTIMSC      0x038 // control interrupt
#define UARTDMACR     0x048 // control dma

#define reg(_reg)	       (volatile u32 *)((_reg) + UART_BASE)
#define write_reg(_reg, value) (*(reg((_reg))) = (value))
#define read_reg(_reg)	       (*(reg((_reg))))

/*
 * Initialize uart
 */
void uart_init(void);

/*
 * Put a single byte to uart
 */
void uart_putchar(u8 c);

/*
 * Put a c string to uart until null is encountered
 */
void uart_print(const u8 *s);

/*
 * Put first n characters of string s
 */
void uart_printn(const u8 *s, usize n);

/*
 * Get byte from uart
 */
u8 uart_getchar(void);

#endif // _UART_H_
