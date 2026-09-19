/* Placeholder kernel layout. The kernel loads at the physical address 1 MiB. */
ENTRY(_start)

SECTIONS
{
    . = 0x100000;

    .text   : ALIGN(4K) { *(.text .text.*) }
    .rodata : ALIGN(4K) { *(.rodata .rodata.*) }
    .data   : ALIGN(4K) { *(.data .data.*) }
    .bss    : ALIGN(4K) { *(COMMON) *(.bss .bss.*) }

    /DISCARD/ : { *(.comment) *(.eh_frame*) *(.note*) }
}
