#ifndef GDT_H
#define GDT_H

struct gdt_entry {
	unsigned short	limit_low;	/* limit, lower 16 bits */
	unsigned short	base_low;	/* base, lower 16 bits */
	unsigned char	base_middle;	/* base, next 8 bits */
	unsigned char	access;		/* flag */
	unsigned char	granularity;	/* flag; limit, last 4 bits */
	unsigned char	base_high;	/* base, last 8 bits */
} __attribute__((packed));		/* no padding */ 

struct gdt_pointer {
	unsigned short	limit;		/* size of gdt - 1 */
	unsigned int	gdt;		/* pointer to gdt */
} __attribute__((packed));

void init_gdt();

#endif
