#include "isr.h"
#include "print.h"
#include "port.h"
#include "string.h"

/* PIC ports */
#define MASTER_COMMAND	0x20
#define MASTER_DATA	0x21
#define SLAVE_COMMAND	0xa0
#define SLAVE_DATA	0xa1
/* EOI (end of interrupt) command */
#define EOI		0x20

/* array of interrupt handlers */
static interrupt_handler handlers[256];

void init_interrupt_handlers()
{
	(void)memset(&handlers, 0, sizeof(interrupt_handler)*256);
}

/* register interrupt handler */
void reg_interrupt_handler(unsigned char interrupt, interrupt_handler handler)
{
	handlers[interrupt] = handler;
}

void isr_handler(struct int_regs regs)
{
	if (handlers[regs.interrupt]) {
		handlers[regs.interrupt](&regs);
	} else {
		print("unhandled interrupt: ");
		print_dec(regs.interrupt);
		print(" error code: ");
		print_dec(regs.error_code);
		print("\n");
	}
}

void irq_handler(struct int_regs regs)
{
	/* acknowledge irq by sending end of interrupt command to PIC */
	/* irq from slave pic? */
	if (regs.interrupt > 7)
		outb(SLAVE_COMMAND, EOI);
	outb(MASTER_COMMAND, EOI);

	if (handlers[regs.interrupt + 32]) {
		handlers[regs.interrupt + 32](&regs);
	} else {
		print("unhandled irq: ");
		print_dec(regs.interrupt);
		print("\n");
	}
}

void enable_irq(unsigned char irq)
{
	unsigned char mask_master, mask_slave;

	/* save masks */
	mask_master = inb(MASTER_DATA);
	mask_slave = inb(SLAVE_DATA);

	/* handled by master or slave pic? */
	if (irq > 7) {
		/* slave */
		outb(SLAVE_DATA, mask_slave & ~(1 << (irq-8)));
	} else {
		/* master */
		outb(MASTER_DATA, mask_master & ~(1 << irq));
	}
}

void disable_irq(unsigned char irq)
{
	unsigned char mask_master, mask_slave;

	/* save masks */
	mask_master = inb(MASTER_DATA);
	mask_slave = inb(SLAVE_DATA);

	/* handled by master or slave pic? */
	if (irq > 7) {
		/* slave */
		outb(SLAVE_DATA, mask_slave | (1 << (irq-8)));
	} else {
		/* master */
		outb(MASTER_DATA, mask_master | (1 << irq));
	}
}
