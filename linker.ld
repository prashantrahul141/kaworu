ENTRY(_start)

SECTIONS
{
    . = 0x40080000;

    .text ALIGN(4K) :
    {
        *(.text.boot)
        *(.text*)
    }

    .rodata ALIGN(4K) :
    {
        *(.rodata*)
    }

    .data ALIGN(4K) :
    {
        *(.data*)
    }

    .bss ALIGN(4K) :
    {
        __bss_start = .;

        *(.bss*)
        *(COMMON)

        __bss_end = .;
    }

    __kernel_end = .;
}
