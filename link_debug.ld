OUTPUT_FORMAT("elf32-i386")
ENTRY(start)
SECTIONS
{
	.text 0x7e00 : {
		* (.text);
	}
	.data : {
		* (.data);
	}
	.rodata : {
		* (.rodata);
	}
	.bss : {
		* (.bss);
	}
}
