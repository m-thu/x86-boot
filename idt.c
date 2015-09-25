#include "idt.h"
#include "string.h"
#include "port.h"
#include "isr.h"

/* IDT flags */
#define SELECTOR 0b1000
#define FLAGS 0b100

/* PIC ports */
#define MASTER_COMMAND	0x20
#define MASTER_DATA	0x21
#define SLAVE_COMMAND	0xa0
#define SLAVE_DATA	0xa1
/* initialisation commands */
#define ICW1_INIT	0x10	/* begin initialisation */
#define ICW1_ICW4	0x01
#define ICW4_8086	0x01

static struct idt_entry idt[256] __attribute__((aligned (8)));
static struct idt_pointer idt_pointer;

static void set_idt_entry(unsigned char index, unsigned int offset,
                          unsigned short selector, unsigned char flags)
{
	idt[index].offset_low = offset & 0xffff;
	idt[index].offset_high = (offset >> 16) & 0xffff;
	idt[index].selector = selector;
	/* interrupt gate */
	idt[index].flags = ((flags << 5) & 0b11100000) | 0b00001110;
	idt[index].reserved = 0;
}

void init_idt()
{
	idt_pointer.limit = sizeof(struct idt_entry)*256 - 1;
	idt_pointer.idt = (unsigned int)&idt;

	(void)memset(&idt, 0, sizeof(struct idt_entry)*256);

	/* selector: first entry, gdt, ring0 (kernel code segment)
	   flags: present, ring0 */
	set_idt_entry(0, (unsigned int)isr0, SELECTOR, FLAGS);
	set_idt_entry(1, (unsigned int)isr1, SELECTOR, FLAGS);
	set_idt_entry(2, (unsigned int)isr2, SELECTOR, FLAGS);
	set_idt_entry(3, (unsigned int)isr3, SELECTOR, FLAGS);
	set_idt_entry(4, (unsigned int)isr4, SELECTOR, FLAGS);
	set_idt_entry(5, (unsigned int)isr5, SELECTOR, FLAGS);
	set_idt_entry(6, (unsigned int)isr6, SELECTOR, FLAGS);
	set_idt_entry(7, (unsigned int)isr7, SELECTOR, FLAGS);
	set_idt_entry(8, (unsigned int)isr8, SELECTOR, FLAGS);
	set_idt_entry(9, (unsigned int)isr9, SELECTOR, FLAGS);
	set_idt_entry(10, (unsigned int)isr10, SELECTOR, FLAGS);
	set_idt_entry(11, (unsigned int)isr11, SELECTOR, FLAGS);
	set_idt_entry(12, (unsigned int)isr12, SELECTOR, FLAGS);
	set_idt_entry(13, (unsigned int)isr13, SELECTOR, FLAGS);
	set_idt_entry(14, (unsigned int)isr14, SELECTOR, FLAGS);
	set_idt_entry(15, (unsigned int)isr15, SELECTOR, FLAGS);
	set_idt_entry(16, (unsigned int)isr16, SELECTOR, FLAGS);
	set_idt_entry(17, (unsigned int)isr17, SELECTOR, FLAGS);
	set_idt_entry(18, (unsigned int)isr18, SELECTOR, FLAGS);
	set_idt_entry(19, (unsigned int)isr19, SELECTOR, FLAGS);
	set_idt_entry(20, (unsigned int)isr20, SELECTOR, FLAGS);
	set_idt_entry(21, (unsigned int)isr21, SELECTOR, FLAGS);
	set_idt_entry(22, (unsigned int)isr22, SELECTOR, FLAGS);
	set_idt_entry(23, (unsigned int)isr23, SELECTOR, FLAGS);
	set_idt_entry(24, (unsigned int)isr24, SELECTOR, FLAGS);
	set_idt_entry(25, (unsigned int)isr25, SELECTOR, FLAGS);
	set_idt_entry(26, (unsigned int)isr26, SELECTOR, FLAGS);
	set_idt_entry(27, (unsigned int)isr27, SELECTOR, FLAGS);
	set_idt_entry(28, (unsigned int)isr28, SELECTOR, FLAGS);
	set_idt_entry(29, (unsigned int)isr29, SELECTOR, FLAGS);
	set_idt_entry(30, (unsigned int)isr30, SELECTOR, FLAGS);
	set_idt_entry(31, (unsigned int)isr31, SELECTOR, FLAGS);

	/* remap IRQs */
	/* begin initialisation */
	outb(MASTER_COMMAND, ICW1_INIT + ICW1_ICW4);
	outb(SLAVE_COMMAND, ICW1_INIT + ICW1_ICW4);
	/* map IRQs 0-7 -> interrupts 32-40 */
	outb(MASTER_DATA, 0x20);
	/* map IRQs 8-15 -> interrupts 41-47 */
	outb(SLAVE_DATA, 0x28);
	/* continue initialisation */
	outb(MASTER_DATA, 4);
	outb(SLAVE_DATA, 2);
	outb(MASTER_DATA, ICW4_8086);
	outb(SLAVE_DATA, ICW4_8086);
	/* mask all IRQs except for IRQ2 on master (connected to slave) */
	outb(MASTER_DATA, ~(1 << 2));
	outb(SLAVE_DATA, 0xff);

	/* IRQs */
	set_idt_entry(32, (unsigned int)irq0, SELECTOR, FLAGS);
	set_idt_entry(33, (unsigned int)irq1, SELECTOR, FLAGS);
	set_idt_entry(34, (unsigned int)irq2, SELECTOR, FLAGS);
	set_idt_entry(35, (unsigned int)irq3, SELECTOR, FLAGS);
	set_idt_entry(36, (unsigned int)irq4, SELECTOR, FLAGS);
	set_idt_entry(37, (unsigned int)irq5, SELECTOR, FLAGS);
	set_idt_entry(38, (unsigned int)irq6, SELECTOR, FLAGS);
	set_idt_entry(39, (unsigned int)irq7, SELECTOR, FLAGS);
	set_idt_entry(40, (unsigned int)irq8, SELECTOR, FLAGS);
	set_idt_entry(41, (unsigned int)irq9, SELECTOR, FLAGS);
	set_idt_entry(42, (unsigned int)irq10, SELECTOR, FLAGS);
	set_idt_entry(43, (unsigned int)irq11, SELECTOR, FLAGS);
	set_idt_entry(44, (unsigned int)irq12, SELECTOR, FLAGS);
	set_idt_entry(45, (unsigned int)irq13, SELECTOR, FLAGS);
	set_idt_entry(46, (unsigned int)irq14, SELECTOR, FLAGS);
	set_idt_entry(47, (unsigned int)irq15, SELECTOR, FLAGS);

	/* init array of interrupt handlers (isr.c) */
	init_interrupt_handlers();

	/* load IDT and enable interrupts */
	asm volatile("lidt %0 \n\t"
		"sti"
		:: "m" (idt_pointer)
		: "memory");
}
