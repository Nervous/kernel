OUTPUT_FORMAT("elf32-i386")

ENTRY(kernel_main)

SECTIONS
{
	. = 0xC0100000 + SIZEOF_HEADERS;

	PROVIDE(_start = .);

	.text : AT(ADDR(.text) - 0xC0000000)
	{
		*(.text)
	}

	.rodata : AT(ADDR(.rodata) - 0xC0000000)
	{
		*(.rodata .rodata.*)
	}

	.ksym : AT(ADDR(.ksym) - 0xC0000000)
	{
		PROVIDE(ksym_start = .);
		*(.ksym)
		PROVIDE(ksym_end = .);
	}

	.modinfo : AT(ADDR(.modinfo) - 0xC0000000)
	{
		PROVIDE(_modinfo_start = .);
		*(.modinfo)
		PROVIDE(_modinfo_end = .);
	}

	.modparam : AT(ADDR(.modparam) - 0xC0000000)
	{
		PROVIDE(_modparam_start = .);
		*(.modparam)
		PROVIDE(_modparam_end = .);
	}

	.data : AT(ADDR(.data) - 0xC0000000)
	{
		*(.data)
	}

	.bss : AT(ADDR(.bss) - 0xC0000000)
	{
		PROVIDE(_bss_start = .);
		*(.bss)
		*(.sbss)
		PROVIDE(_bss_end = .);
	}

	PROVIDE(_end = .);

	. = 0x0;

	.cpuvar (OVERLAY) :
	{
		*(.cpuvar)
	}
}
