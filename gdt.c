#include "gdt.h"

static struct gdt_entry gdt[5] __attribute__((aligned (8)));
static struct gdt_pointer gdt_pointer;

static void set_gdt_entry(int index, unsigned int base, unsigned int limit,
                          unsigned char access, unsigned char granularity)
{
	gdt[index].base_low = base & 0xffff;
	gdt[index].base_middle = (base >> 16) & 0xff;
	gdt[index].base_high = (base >> 24) & 0xff;
	gdt[index].limit_low = limit & 0xffff;
	gdt[index].granularity = (limit >> 16) & 0xf;
	gdt[index].access = access;
	gdt[index].granularity |= (granularity << 4) & 0xf0;
}

void init_gdt()
{
	gdt_pointer.gdt = (unsigned int)&gdt;
	gdt_pointer.limit = sizeof(struct gdt_entry)*5 - 1;

	/* first entry is always zero */
	set_gdt_entry(0, 0, 0, 0, 0);
	/* kernel code segment:
	   present, ring0, code or data, code, execute/read
	   4k granularity, 32 bit, always zero, available */
	set_gdt_entry(1, 0x0, 0xfffff, 0b10011010, 0b1100);
	/* kernel data segment:
	   present, ring0, code or data, data, read/write */
	set_gdt_entry(2, 0x0, 0xfffff, 0b10010010, 0b1100);
	/* user space code segment:
	   present, ring3, code or data, code, execute/read */
	set_gdt_entry(3, 0x0, 0xfffff, 0b11111010, 0b1100);
	/* user space data segment:
	   present, ring3, code or data, data, read/write */
	set_gdt_entry(4, 0x0, 0xfffff, 0b11110010, 0b1100);

	/* load GDT and update segment registers */
	asm volatile("lgdt %0 \n\t"
		"movw $0b10000, %%ax \n\t"	/* ring0, gdt, second entry */
		"movw %%ax, %%ds \n\t"
		"movw %%ax, %%es \n\t"
		"movw %%ax, %%fs \n\t"
		"movw %%ax, %%gs \n\t"
		"movw %%ax, %%ss \n\t"
		"ljmp $0b1000, $1f \n\t"	/* ring0, gdt, first entry */
		"1:"
		:: "m" (gdt_pointer)
		: "%ax", "memory");
}
