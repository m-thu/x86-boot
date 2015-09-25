#include "timer.h"
#include "isr.h"
#include "port.h"

/* PIT command port */
#define PIT_COMMAND 0x43
/* data port channel 0 */
#define PIT_CHANNEL0 0x40

static unsigned long long ticks = 0;

static void pit_handler(struct int_regs *regs)
{
	++ticks;
}

void init_timer(unsigned int frequency)
{
	/* PIT frequency: 1193180 Hz */
	unsigned short divisor = 1193180 / frequency;

	/* disable IRQ0 */
	disable_irq(0);

	/* 00: channel 0, 11: access mode lowbyte/highbyte,
	   011: mode 3 (square wave), 0: 16-bit binary */
	outb(PIT_COMMAND, 0b00110110);
	/* send divisor */
	outb(PIT_CHANNEL0, divisor & 0xff);		/* low byte */
	outb(PIT_CHANNEL0, (divisor >> 8) & 0xff);	/* high byte */

	/* register interrupt handler and enable IRQ0 */
	reg_interrupt_handler(32 + 0, &pit_handler);
	enable_irq(0);
}
