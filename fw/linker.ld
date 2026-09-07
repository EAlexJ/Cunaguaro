MEMORY
{
    RAM (rwx) : ORIGIN = 0x40000000, LENGTH = 256K  /* Data RAM */
}

SECTIONS
{
    .text : {
        *(.text.init)    /* Reset and boot code first */
        *(.text .text.*)
        *(.rodata .rodata.*)
    } > RAM

    .data : AT(ADDR(.text) + SIZEOF(.text)) {
        __data_start = .;
        *(.data .data.*)
        *(.sdata .sdata.*)
        __data_end = .;
    } > RAM
    __data_load_start = LOADADDR(.data);

    /* Zero-initialized globals */
    .bss : {
        __bss_start = .;
        *(.bss .bss.*)
        *(.sbss .sbss.*)
        *(COMMON)
        __bss_end = .;
    } > RAM

    /* Stack allocated at the top of RAM */
    __stack_top = ORIGIN(RAM) + LENGTH(RAM);
}