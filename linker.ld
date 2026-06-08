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
        . = ALIGN(0x1000);
        PROVIDE(__kernel_text_end = .);
    }

    . = ALIGN(16);
    .rodata : {
        . = ALIGN(16);
        *(.srodata .srodata.*)
        . = ALIGN(16);
        *(.rodata .rodata.*)
    }

    . = ALIGN(16);
    .data : {
        . = ALIGN(16);
        *(.sdata .sdata.*)
        . = ALIGN(16);
        *(.data .data.*)
    }

    . = ALIGN(16);
    .bss : {
        __bss_start = .;
        . = ALIGN(16);
        *(.sbss .sbss.*)
        . = ALIGN(16);
        *(.bss .bss.*)
        . = ALIGN(16);
        *(COMMON);
        __bss_end = .;
    }

    # end kernel on a page boundary
    . = ALIGN(0x1000);

    PROVIDE(__kernel_end = .);
}
