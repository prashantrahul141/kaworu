OUTPUT_FORMAT(elf64-littleaarch64)

ENTRY(start)

PHDRS
{
    limine_requests PT_LOAD;
    text PT_LOAD;
    rodata PT_LOAD;
    data PT_LOAD;
}

SECTIONS
{

    . = 0xffffffff80000000;
    PROVIDE(__kernel_start = .);
    .limine_requests : {
        KEEP(*(.limine_requests_start))
        KEEP(*(.limine_requests))
        KEEP(*(.limine_requests_end))
    } :limine_requests

     . = ALIGN(CONSTANT(MAXPAGESIZE));
    PROVIDE(__kernel_text_start = .);
    .text.boot : {
        *(.text.boot)
    }

     . = ALIGN(CONSTANT(MAXPAGESIZE));
    .text : {
        *(.text*)
        . = ALIGN(0x1000);
    }
    PROVIDE(__kernel_text_end = .);

    . = ALIGN(CONSTANT(MAXPAGESIZE));
    PROVIDE(__kernel_data_start = .);
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

    . = ALIGN(CONSTANT(MAXPAGESIZE));
    PROVIDE(__kernel_data_end = .);

    /* Discard .note.* and .eh_frame* since they may cause issues on some hosts. */
    /DISCARD/ : {
        *(.eh_frame*)
        *(.note .note.*)
    }

    /* end kernel on a page boundary */
    . = ALIGN(0x1000);
    PROVIDE(__kernel_end = .);
}
