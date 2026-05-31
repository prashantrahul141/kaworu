.section .text.boot
.global _start

.extern kernel_main

_start:
    ldr x0, =stack_top
    mov sp, x0

    bl kernel_main

1:
    wfe
    b 1b

.section .bss

.align 12
stack_bottom:
    .skip 16384
stack_top:
