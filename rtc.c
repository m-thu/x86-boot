/* http://www.nondot.org/sabre/os/files/MiscHW/RealtimeClockFAQ.txt */

#include "rtc.h"
#include "port.h"
#include "isr.h"
#include "print.h"

#define CMOS_ADDRESS	0x70
#define CMOS_DATA	0x71

/* time in 24 hour binary format */
struct time time;

#define BCD 1
#define BINARY 0
unsigned char mode;
#define H12 1
#define H24 0
unsigned char hour_format;

struct time *get_time()
{
	return &time;
}

static unsigned char bcd_to_bin(unsigned char c)
{
	return (c / 16) * 10 + (c & 0xf);
}

/* IRQ 8 handler */
static void update_time()
{
	unsigned char c, x, y;

	/* read status register c to acknowledge interrupt */
	outb(CMOS_ADDRESS, 0xc);
	c = inb(CMOS_DATA);

	/* wait for clock update to complete */
	do {
		/* select register a */
		outb(CMOS_ADDRESS, 0xa);
		/* get value */
		c = inb(CMOS_DATA);
	} while (c & (1 << 7));

	/* read time */
	outb(CMOS_ADDRESS, 0x0);
	time.second = inb(CMOS_DATA);
	outb(CMOS_ADDRESS, 0x2);
	time.minute = inb(CMOS_DATA);
	outb(CMOS_ADDRESS, 0x4);
	time.hour = inb(CMOS_DATA);
	outb(CMOS_ADDRESS, 0x6);
	time.day_of_week = inb(CMOS_DATA);
	outb(CMOS_ADDRESS, 0x7);
	time.day = inb(CMOS_DATA);
	outb(CMOS_ADDRESS, 0x8);
	time.month = inb(CMOS_DATA);
	outb(CMOS_ADDRESS, 0x9);
	time.year = inb(CMOS_DATA);
	outb(CMOS_ADDRESS, 0x32);
	time.century = inb(CMOS_DATA);

	/* convert time to 24h binary format */
	/* if 12h and pm mask off 0x80 bit (both binary and bcd format) */
	if ((hour_format == H12) && (time.hour & 0x80))
		time.hour &= ~0x80;
	/* convert bcd to binary */
	if (mode == BCD) {
		time.second = bcd_to_bin(time.second);
		time.minute = bcd_to_bin(time.minute);
		time.hour = bcd_to_bin(time.hour);
		time.day = bcd_to_bin(time.day);
		time.month = bcd_to_bin(time.month);
		time.year = bcd_to_bin(time.year);
		time.century = bcd_to_bin(time.century);
	}

	x = get_cursor_x();
	y = get_cursor_y();
	set_cursor(80 - 8, 0);
	if (time.second < 10)
		print(" ");
	printf("%i:%i:%i", time.hour, time.minute, time.second);
	set_cursor(x, y);
}

void init_rtc()
{
	unsigned char b;

	asm volatile("cli" ::: "memory");

	/* get value of status register b */
	outb(CMOS_ADDRESS, 0xb);
	b = inb(CMOS_DATA);
	hour_format = b & (1 << 1) ? 0 : 1;
	mode = b & (1 << 2) ? 0 : 1;

	/* initial update */
	update_time();

	/* enable update-ended interrupt */
	b |= (1 << 4);
	outb(CMOS_ADDRESS, 0xb);
	outb(CMOS_DATA, b);

	/* register irq handler and enable IRQ 8 */
	reg_interrupt_handler(32 + 8, &update_time);
	enable_irq(8);

	asm volatile("sti" ::: "memory");

	printf("rtc mode = %s, hour_format = %s\n", mode==BCD?"bcd":"binary", hour_format==H12?"12h":"24h");
}
