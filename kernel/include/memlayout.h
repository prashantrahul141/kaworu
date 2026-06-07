#ifndef __MEMLAYOUT_H_
#define __MEMLAYOUT_H_

#include "config.h"
#include "types.h"

/*
 * 0x0F0XXXXX   free bottom top
 * ....
 * ....
 * ....       + MEMORY SIZE - kernel size
 * ....
 * ....
 * 0x040XXXXX   kernel_top = free mem bottom (kernel ends here)
 * ....
 * ....       + .bss
 * ....       + .data
 * ....       + .rodata
 * ....       + .text
 * ....
 * 0x40100000   kernel_bottom (kernel starts here)
 *
 * 0x00000000   absolute zero
 */

extern symbol __kernel_end;
extern symbol __kernel_start;

#define __KERNEL_START ((usize)__kernel_start)
#define __KERNEL_END   ((usize)__kernel_end)

#define MB_TO_BYTES(MB) ((MB) * 1000 * 1000)
#define KERNEL_BOTTOM	(__KERNEL_START)
#define KERNEL_TOP	(__KERNEL_END)
#define FREE_MEM_BOTTOM (KERNEL_TOP)
#define FREE_MEM_TOP \
	((KERNEL_BOTTOM) + (MB_TO_BYTES(CONFIG_PHYSICAL_MEMORY_MB)))

#define PAGE_SIZE 4096

#define UART_BASE 0x09000000

#endif // __MEMLAYOUT_H_
