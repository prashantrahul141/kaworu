#ifndef __MEMLAYOUT_H_
#define __MEMLAYOUT_H_

#include "config.h"
#include "types.h"

/*
 * 0x0F0XXXXX   free mem top
 * ....
 * ....
 * ....       + MEMORY SIZE - kernel size
 * ....
 * ....
 * 0x040XXXXX   kernel_top = free mem bottom (kernel ends here)
 * ....
 * ....       + .bss
 * ....       + .data
 * ....       + .rodata -> __KERNEL_TEXT_END
 * ....       + .text
 * ....
 * 0x40100000   kernel_bottom (kernel starts here)
 *
 * 0x00000000   absolute zero
 */

extern symbol __kernel_end;
extern symbol __kernel_start;
extern symbol __kernel_text_start;
extern symbol __kernel_text_end;
extern symbol __kernel_data_start;
extern symbol __kernel_data_end;

#define __KERNEL_START	    ((usize)__kernel_start)
#define __KERNEL_END	    ((usize)__kernel_end)
#define __KERNEL_TEXT_START ((usize)__kernel_text_start)
#define __KERNEL_TEXT_END   ((usize)__kernel_text_end)
#define __KERNEL_DATA_START ((usize)__kernel_data_start)
#define __KERNEL_DATA_END   ((usize)__kernel_data_end)

#define MB_TO_BYTES(MB) ((MB) * (usize)1000 * (usize)1000)

constexpr usize PAGE_SIZE = 4096;

constexpr usize UART_BASE = 0x09000000;

#endif // __MEMLAYOUT_H_
