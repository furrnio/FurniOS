ENTRY(reset)
SECTIONS
{
    . = 0x00000000;

    .vector_table : { KEEP(*(.vector_table)) }

	. = 0x00008000;

	.init : { *(.init) }
	.text : { *(.text) }
	.rodata : { *(.rodata) }
	. = ALIGN(1<<20);
	.data : { *(.data) }
	.bss  : { *(.bss)  }
}
