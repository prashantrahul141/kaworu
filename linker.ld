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
        *(.bss*);
        *(COMMON);
        . += 16384;
        . = ALIGN(16);
        __bss_end = .;
    }

    __kernel_end = .;
}
