#include "print.h"
#include "colors.h"
#include "port.h"
#include "stdarg.h"

/* cursor position */
static unsigned char x = 0, y = 0;
/* color video ram */
static volatile unsigned short *videoram = (unsigned short *)0xb8000;

static void cursor()
{
	unsigned short pos = y*80 + x;
	
	outb(0x3d4, 0x0f);
	outb(0x3d5, pos & 0xff);		/* low byte */
	outb(0x3d4, 0x0e);
	outb(0x3d5, (pos >> 8) & 0xff);		/* high byte */
}

static void scroll()
{
	/*int i;*/
	unsigned int i0, i1, i2;

	/*for (i = 0; i < 80*24; ++i)
		videoram[i] = videoram[i + 80];*/
	asm volatile("cld \n\t"
		"rep movsw"
		: "=&S" (i0), "=&D" (i1), "=&c" (i2)
		: "0" (videoram + 80), "1" (videoram), "2" (80*24));
	/*for (i = 80*24; i < 80*25; ++i)
		videoram[i] = ' ' | (c_light_grey << 8) | (c_black << 12);*/
	asm volatile("rep stosw"
		: "=&D" (i0), "=&c" (i1)
		: "0" (videoram + 80*24), "a" (' ' | (c_light_grey << 8) |
		  (c_black << 12)), "1" (80));
}

void cls()
{
	/*int i;*/
	unsigned int i0, i1;

	/*for (i = 0; i < 80*25; ++i)
		videoram[i] = ' ' | (c_light_grey << 8) | (c_black << 12);*/
	asm volatile("cld \n\t"
		"rep stosw"
		: "=&D" (i0), "=&c" (i1)
		: "0" (videoram), "a" (' ' | (c_light_grey << 8) |
		  (c_black << 12)), "1" (80*25));
	x = 0; y = 0;
	cursor();
}

void print_char(char c)
{
	if (c == '\n') {
		++y;
		x = 0;
	} else {
		videoram[y*80 + x] = c | (c_light_grey << 8) | (c_black << 12);
		++x;
	}

	if (x == 80) {
		++y;
		x = 0;
	}
	if (y == 25) {
		scroll();
		y = 24;
	}
	cursor();
}

void print(char *s)
{
	while (*s) {
		print_char(*s);
		++s;
	}
}

void print_dec(unsigned int i)
{
	if (i > 9) {
		print_dec(i / 10);
		print_dec(i % 10);
	} else {
		print_char(i + '0');
	}
}

void print_hex(unsigned int i)
{
	if (i > 0xf) {
		print_hex(i / 16);
		print_hex(i % 16);
	} else {
		print_char(i<0xa ? i + '0' : i - 0xa + 'a');
	}
}

void print_bin(unsigned int i)
{
	if (i > 1) {
		print_bin(i / 2);
		print_bin(i % 2);
	} else {
		print_char(i ? '1' : '0');
	}
}

/* format string:
   %i,%d: decimal, %x: hex, %b binary
   %c: char, %s: string */
void printf(char *fmt, ...)
{
	/* list of arguments */
	va_list arg;

	/* va_start(argument list, last argument) */
	va_start(arg, fmt);

	while (*fmt) {
		if (*fmt == '%') {
			switch (fmt[1]) {
			case 'i':
			case 'd':
				print_dec(va_arg(arg, int));
				fmt += 2;
				continue;
			case 'x':
				print_hex(va_arg(arg, int));
				fmt += 2;
				continue;
			case 'b':
				print_bin(va_arg(arg, int));
				fmt += 2;
				continue;
			case 'c':
				/* char is passed as int by va_arg */
				print_char((char)va_arg(arg, int));
				fmt += 2;
				continue;
			case 's':
				print(va_arg(arg, char *));
				fmt += 2;
				continue;
			default:
				break;
			}
		}
		print_char(*fmt);
		++fmt;
	}

	va_end(arg);
}

unsigned char get_cursor_x()
{
	return x;
}

unsigned char get_cursor_y()
{
	return y;
}

void set_cursor(unsigned char _x, unsigned char _y)
{
	x = _x;
	y = _y;
	cursor();
}

void init_cursor()
{
	/* get cursor position saved in boot loader */
	x = *((unsigned char *)(0x7c00 + 510));
	y = *((unsigned char *)(0x7c00 + 510 + 1));
	cursor();
}
