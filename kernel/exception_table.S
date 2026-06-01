/*
 * Exception register frame
 *
 *  offset |  register
 * -------------------------
 *    0    |  x0   x1
 *   16    |  x2   x3
 *   32    |  x4   x5
 *   48    |  x6   x7
 *   64    |  x8   x9
 *   80    |  x10  x11
 *   96    |  x12  x13
 *  112    |  x14  x15
 *  128    |  x16  x17
 *  144    |  x18  x19
 *  160    |  x20  x21
 *  176    |  x22  x23
 *  192    |  x24  x25
 *  208    |  x26  x27
 *  224    |  x28  x29
 *  240    |  x30  ELR_EL1
 *  256    |  SPSR_EL1  ESR_EL1
 *  272    |  FAR_EL1
 *
 * total size = 288
 */

/*
 * Save register state on stack
 * macro save_registers();
 * returns:
 *       x0 => sp
 */
.macro save_registers
    sub sp, sp, #288

    stp x0,  x1,  [sp, #0]
    stp x2,  x3,  [sp, #16]
    stp x4,  x5,  [sp, #32]
    stp x6,  x7,  [sp, #48]
    stp x8,  x9,  [sp, #64]
    stp x10, x11, [sp, #80]
    stp x12, x13, [sp, #96]
    stp x14, x15, [sp, #112]
    stp x16, x17, [sp, #128]
    stp x18, x19, [sp, #144]
    stp x20, x21, [sp, #160]
    stp x22, x23, [sp, #176]
    stp x24, x25, [sp, #192]
    stp x26, x27, [sp, #208]
    stp x28, x29, [sp, #224]

    mrs x0, ELR_EL1
    stp x30, x0, [sp, #240]

    mrs x0, SPSR_EL1
    mrs x1, ESR_EL1
    stp x0, x1, [sp, #256]

    mrs x0, FAR_EL1
    str x0, [sp, #272]

    mov x0, sp
.endm


/*
 * Restores register state from stack
 * macro restore_registers();
 */
.macro restore_registers
    ldp x30, x2, [sp, #240]
    msr ELR_EL1, x2

    ldr x2, [sp, #256]
    msr SPSR_EL1, x2

    ldp x28, x29, [sp, #224]
    ldp x26, x27, [sp, #208]
    ldp x24, x25, [sp, #192]
    ldp x22, x23, [sp, #176]
    ldp x20, x21, [sp, #160]
    ldp x18, x19, [sp, #144]
    ldp x16, x17, [sp, #128]
    ldp x14, x15, [sp, #112]
    ldp x12, x13, [sp, #96]
    ldp x10, x11, [sp, #80]
    ldp x8,  x9,  [sp, #64]
    ldp x6,  x7,  [sp, #48]
    ldp x4,  x5,  [sp, #32]
    ldp x2,  x3,  [sp, #16]
    ldp x0,  x1,  [sp, #0]

    add sp, sp, #288
.endm

.extern exception_handler
.extern irq_handler
.extern unhandled_exception_vector

/* Exception vector table */
.section .text
.balign 0x800
vector_table_el1:
/*
 * The exception handler for a synchronous
 * exception from the current EL using SP0.
 */
curr_el_sp0_sync:
    save_registers
    bl unhandled_exception_vector
    restore_registers
    eret

/*
 * The exception handler for an IRQ exception
 * from the current EL using SP0.
 */
.balign 0x80
curr_el_sp0_irq:
    save_registers
    bl unhandled_exception_vector
    restore_registers
    eret

/*
 *
 * The exception handler for an FIQ exception
 * from the current EL using SP0.
*/
.balign 0x80
curr_el_sp0_fiq:
    save_registers
    bl unhandled_exception_vector
    restore_registers
    eret

/*
 * The exception handler for a System Error
 * exception from the current EL using SP0.
 */
.balign 0x80
curr_el_sp0_serror:
    save_registers
    bl unhandled_exception_vector
    restore_registers
    eret

/*
 * The exception handler for a synchrous
 * exception from the current EL using the current SP
 */

.balign 0x80
curr_el_spx_sync:
    save_registers
    bl exception_handler
    restore_registers
    eret

/*
 * The exception handler for an IRQ exception from
 * the current EL using the current SP.
 */
.balign 0x80
curr_el_spx_irq:
    save_registers
    bl irq_handler
    restore_registers
    eret

/*
 * The exception handler for an FIQ from
 * the current EL using the current SP.
 */
.balign 0x80
curr_el_spx_fiq:
    save_registers
    bl unhandled_exception_vector
    restore_registers
    eret

/*
 * The exception handler for a System Error
 * exception from the current EL using the current SP.
 */
.balign 0x80
curr_el_spx_serror:
    save_registers
    bl unhandled_exception_vector
    restore_registers
    eret

/*
 * The exception handler for a synchronous
 * exception from a lower EL (AArch64).
 */
 .balign 0x80
lower_el_aarch64_sync:
    save_registers
    bl exception_handler
    restore_registers
    eret

/*
 * The exception handler for an IRQ from a lower EL
 * (AArch64).
 */
.balign 0x80
lower_el_aarch64_irq:
    save_registers
    bl unhandled_exception_vector
    restore_registers
    eret

/*
 * The exception handler for an FIQ from a lower EL
 * (AArch64).
 */
.balign 0x80
lower_el_aarch64_fiq:
    save_registers
    bl unhandled_exception_vector
    restore_registers
    eret

/*
 * The exception handler for a System Error
 * exception from a lower EL(AArch64).
 */
.balign 0x80
lower_el_aarch64_serror:
    save_registers
    bl unhandled_exception_vector
    restore_registers
    eret

/*
 * The exception handler for a synchronous
 * exception from a lower EL(AArch32).
 */
.balign 0x80
lower_el_aarch32_sync:
    save_registers
    bl unhandled_exception_vector
    restore_registers
    eret

/*
 * The exception handler for an IRQ exception
 * from a lower EL (AArch32).
 */
.balign 0x80
lower_el_aarch32_irq:
    save_registers
    bl unhandled_exception_vector
    restore_registers
    eret

/*
 * The exception handler for an FIQ exception from
 * a lower EL (AArch32).
 */
.balign 0x80
lower_el_aarch32_fiq:
    save_registers
    bl unhandled_exception_vector
    restore_registers
    eret

/*
 * The exception handler for a System Error
 * exception from a lower EL(AArch32).
 */
.balign 0x80
lower_el_aarch32_serror:
    save_registers
    bl unhandled_exception_vector
    restore_registers
    eret

/*
 * Disables all interrupts
 * macro disable_interrupts(void);
 */
.macro disable_interrupts
    /* mask D, A, I, F */
    msr daifset, #0xf
.endm

/*
 * Enables all interrupts
 * void enable_interrupts(void);
 */
.macro enable_interrupts
    /* unmask D, A, I, F */
    msr daifclr, #0xf
.endm
