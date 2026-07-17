#include "arch/aarch64/exception.S"
#include "config.h"

.section .bss

.align 16
boot_stack:
    .skip CONFIG_BOOT_STACK_SIZE
boot_stack_top:

.section .text.boot

.extern kernel_main

.global start
start:
    /* disable all interrupts */
    /* mask (NOT D), A, I, F */
    msr daifclr, #0b1000
    msr daifset, #0b0111

    /* use current SP_ELx (1) for sp */
    mov x0, #1
    msr SPSel, x0
    isb

    /* setup boot stack */
    ldr x0, =boot_stack_top
    mov sp, x0
    isb

    /* setup exception vector table */
    ldr x0, =vector_table_el1
    msr vbar_el1, x0
    isb

    /* System control register: enable fpu and simd */
    mrs x0, cpacr_el1
    orr x0, x0, #(0x3 << 20)
    msr cpacr_el1, x0
    isb

    /* reenable all interrupts */
    /* unmask D, A, I, F */
    msr daifclr, #0b1111
    isb

    /* c entry point */
    bl kernel_main

    /* unreachable */
halt:
    wfe
    b halt
