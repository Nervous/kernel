OUTPUT_FORMAT("elf32-i386")

ENTRY(_start)

SECTIONS
{
	. = 0x300000;
	/*. = 0x7E00;*/
	.multiboot : { *(.multiboot) }
	.text : { *(.text) }
}
