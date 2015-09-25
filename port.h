#ifndef PORT_H
#define PORT_H

static inline __attribute__((always_inline, unused))
void outb(unsigned short port, unsigned char value)
{
	asm volatile("outb %1, %0"
		:: "Nd" (port), "a" (value));
}

static inline __attribute__((always_inline, unused))
unsigned char inb(unsigned short port)
{
	unsigned char value;

	asm volatile("inb %1, %0"
		: "=a" (value)
		: "Nd" (port));
	return value;
}

#endif
