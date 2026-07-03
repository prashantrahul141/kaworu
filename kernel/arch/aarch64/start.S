#include "arch/aarch64/exception.S"
#include "config.h"

.section .text.boot

.extern kernel_main

.global start
start:
    /* disable all interrupts */
    disable_interrupts

    /* setup exception vector table */
    ldr x0, =vector_table_el1
    msr vbar_el1, x0
    isb

    /* System control register: enable fpu and simd */
    mrs x0, cpacr_el1
    orr x0, x0, #(0x3 << 20)
    msr cpacr_el1, x0

    /* reenable all interrupts */
    enable_interrupts

    /* c entry point */
    bl kernel_main

    /* unreachable */
halt:
    wfe
    b halt
