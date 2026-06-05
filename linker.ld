ENTRY(_start)

SECTIONS
{

    . = 0x40100000;
    PROVIDE(__kernel_start = .);

    .text.boot : {
        *(.text.boot)
    }

    .text : {
        *(.text*)
    }

    .rodata : {
        . = ALIGN(16);
        *(.srodata .srodata.*)
        . = ALIGN(16);
        *(.rodata .rodata.*)
    }

    .data : {
        . = ALIGN(16);
        *(.sdata .sdata.*)
        . = ALIGN(16);
        *(.data .data.*)
    }

    . = ALIGN(16);
    .bss : {
        __bss_start = .;
        *(.bss*);
        *(COMMON);
        . = ALIGN(16);
        __bss_end = .;
    }

    PROVIDE(__kernel_end = .);
}
