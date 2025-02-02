OUTPUT_ARCH("riscv")
ENTRY(_entry)

SECTIONS {
    /* ensure that _entry is starting at 0x80000000 */
    . = 0x80000000; 

    .text : {
        *(.text .text.*)
        . = ALIGN(0x1000);

        trampoline = .;
        *(trampsec)
        . = ALIGN(0x1000);

        ASSERT(. - trampoline == 0x1000, "trampoline segment is not page align");
        PROVIDE(etext = .);
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
        *(.rodata .rodata.*)
    }  

    .bss : {
        . = ALIGN(16);
        *(.sbss .sbss.*)
        . = ALIGN(16);
        *(.bss .bss.*)
    } 

    PROVIDE(end = .);
}