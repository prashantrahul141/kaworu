.section .text.boot

.extern kernel_main

.global _start
_start:
    # zero out bss section
    adr x1, __bss_start
    adr x0, __bss_end
    sub x1, x0, x1
    bl memzero

    # move stack_top to sp
    ldr x0, =stack_top
    mov sp, x0

    # c entry point
    bl kernel_main

    # unreachable
    b halt


/*
 * zero out a memory region
 * start_address should be the higer value.
 * void memzero(char *start_address, char* size);
 */
memzero:
    /*  *x0 = 0;
        x0 += 8;  */
    str xzr, [x0], #8

    /* x1 -= 8 */
    subs x1, x1, #8

    /* if x1 > 0: goto memzero */
    b.gt memzero

    ret

/*
 * sleeps indefinitely.
 * void halt()
 */
halt:
    wfe
    b halt


.section .bss
.align 16
stack_bottom:
    .skip 16384
stack_top:
