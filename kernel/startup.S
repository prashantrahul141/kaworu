#include "entry.S"
#include "config.h"

.section .text.boot

.extern kernel_main

/*
 * zero out a memory region
 * start_address should be the lower memory address.
 * macro memzero(char *start_address, char* size);
 */
.macro memzero start, size
1:
    str xzr, [\start], #8
    subs \size, \size, #8
    b.gt 1b
.endm

/*
 * sleeps indefinitely.
 * macro halt(void);
 */
.macro halt
1:
    wfe
    b 1b
.endm


.global _start
_start:
    /* setup unique stack for each cpu */
    mrs x0, MPIDR_EL1
    and x0, x0, #0xFF

    adrp x1, stack0
    add  x1, x1, :lo12:stack0

    add  x3, x0, #1

    mov  x2, #CONFIG_PER_CPU_STACK_SIZE
    mul  x3, x3, x2

    add  sp, x1, x3

    /* spin all cpus except cpu0 */
    cbz x0, boot

spin:
    halt

boot:
    /* get current EL level */
    mrs x0, CurrentEL
    lsr x0, x0, #2
    cmp x0, #2
    beq in_el2
    cmp x0, #1
    beq in_el1

in_el3:
    /* jump from el3 to el2 */

    /* store address of `in_el2` in ELR_EL3 */
    adr x0, in_el2
    msr ELR_EL3, x0


    /* store el level we want to jump, also sets el2's sp to SP_EL2 */
    mov x0, #0x3c9
    msr SPSR_EL3, x0
    /* copy this el's sp to el2's sp */
    mov x0, sp
    msr SP_EL2, x0

    eret

in_el2:
    /* jump from el2 to el1 */
    /* store address of `in_el1` in ELR_EL2 */
    adr x0, in_el1
    msr ELR_EL2, x0

    /* store el level we want to jump, also sets el1's sp to SP_EL1 */
    mov x0, #0x3c5
    msr SPSR_EL2, x0

    /* copy this el's sp to el1's sp */
    mov x0, sp
    msr SP_EL1, x0

    eret

in_el1:
    /* finnaly in el1 */
    /* disable all interrupts */
    disable_interrupts

    /* zero out bss section */
    ldr x0, =__bss_start
    ldr x1, =__bss_end
    sub x1, x1, x0
    memzero x0, x1

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
    halt
