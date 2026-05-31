ENTRY(_start)

SECTIONS
{
    . = 0x40100000;

    .text.boot : {
        *(.text.boot)
    }

   .text : {
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data*)
    }

    . = ALIGN(16);
    .bss : {
        __bss_start = .;
        *(.bss*)
        __bss_end = .;
    }

    __kernel_end = .;
}
