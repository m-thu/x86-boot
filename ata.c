/* http://wiki.osdev.org/ATA_PIO_Mode */

#include "ata.h"
#include "port.h"
#include "print.h"
#include "isr.h"

void read_lba48(unsigned int lba_low, unsigned int lba_high,
                unsigned short count, unsigned char *buf)
{
	unsigned char c;
	unsigned int i0, i1;
	int i;

	/* LBA, master */
	outb(PRIMARY + REG_DRIVE, (1 << 6));
	/* sector count high byte */
	outb(PRIMARY + REG_SEC_COUNT, (count >> 8) & 0xff);
	/* LBA high */
	outb(PRIMARY + REG_SEC_NUM, (lba_low >> 24) & 0xff);
	outb(PRIMARY + REG_CYL_LOW, lba_high & 0xff);
	outb(PRIMARY + REG_CYL_HIGH, (lba_high >> 8) & 0xff);
	/* sector count low byte */
	outb(PRIMARY + REG_SEC_COUNT, count & 0xff);
	/* LBA low */
	outb(PRIMARY + REG_SEC_NUM, lba_low & 0xff);
	outb(PRIMARY + REG_CYL_LOW, (lba_low >> 8) & 0xff);
	outb(PRIMARY + REG_CYL_HIGH, (lba_low >> 16) & 0xff);
	/* send read sectors command */
	outb(PRIMARY + REG_CMD_STAT, CMD_READ_EXT);
	for (i = 0; i < count; ++i) {
		/* delay 400 ns */
		(void)inb(PRIMARY_CTRL);
		(void)inb(PRIMARY_CTRL);
		(void)inb(PRIMARY_CTRL);
		(void)inb(PRIMARY_CTRL);
		/* wait for command to complete */
		do {
			c = inb(PRIMARY_CTRL);
			if ((c & STAT_ERR) || (c & STAT_DF))
				break;
		} while ((c & STAT_BSY) && !(c & STAT_DRQ));
		if ((c & STAT_ERR) || (c & STAT_DF))
			print("ata: drive error\n");
		/* read data into buffer */
		asm volatile("cld \n\t"
			"rep insw"
			: "=&D" (i0), "=&c" (i1)
			: "0" (buf), "1" (256), "d" (PRIMARY + REG_DATA)
			: "memory");
		/* next sector */
		buf += 512;
	}
}

void write_lba48(unsigned int lba_low, unsigned int lba_high,
                 unsigned short count, unsigned char *buf)
{
	unsigned char c;
	int i, j;

	/* LBA, master */
	outb(PRIMARY + REG_DRIVE, (1 << 6));
	/* sector count high byte */
	outb(PRIMARY + REG_SEC_COUNT, (count >> 8) & 0xff);
	/* LBA high */
	outb(PRIMARY + REG_SEC_NUM, (lba_low >> 24) & 0xff);
	outb(PRIMARY + REG_CYL_LOW, lba_high & 0xff);
	outb(PRIMARY + REG_CYL_HIGH, (lba_high >> 8) & 0xff);
	/* sector count low byte */
	outb(PRIMARY + REG_SEC_COUNT, count & 0xff);
	/* LBA low */
	outb(PRIMARY + REG_SEC_NUM, lba_low & 0xff);
	outb(PRIMARY + REG_CYL_LOW, (lba_low >> 8) & 0xff);
	outb(PRIMARY + REG_CYL_HIGH, (lba_low >> 16) & 0xff);
	/* send write sectors command */
	outb(PRIMARY + REG_CMD_STAT, CMD_WRITE_EXT);
	for (i = 0; i < count; ++i) {
		/* delay 400 ns */
		(void)inb(PRIMARY_CTRL);
		(void)inb(PRIMARY_CTRL);
		(void)inb(PRIMARY_CTRL);
		(void)inb(PRIMARY_CTRL);
		/* wait for command to complete */
		do {
			c = inb(PRIMARY_CTRL);
			if ((c & STAT_ERR) || (c & STAT_DF))
				break;
		} while ((c & STAT_BSY) && !(c & STAT_DRQ));
		if ((c & STAT_ERR) || (c & STAT_DF))
			print("ata: drive error\n");
		/* write data from buffer */
		for (j = 0; j < 256; ++j) {
			asm volatile("outw %%ax, %%dx"
				:: "a" (((unsigned short *)buf)[j]),
				   "d" (PRIMARY + REG_DATA)
				: "memory");
		}
		/* next sector */
		buf += 512;
	}
	/* delay 400 ns */
	(void)inb(PRIMARY_CTRL);
	(void)inb(PRIMARY_CTRL);
	(void)inb(PRIMARY_CTRL);
	(void)inb(PRIMARY_CTRL);
	/* wait for command to complete */
	do {
		c = inb(PRIMARY_CTRL);
		if ((c & STAT_ERR) || (c & STAT_DF))
			break;
	} while ((c & STAT_BSY) && !(c & STAT_DRQ));
	if ((c & STAT_ERR) || (c & STAT_DF))
		print("ata: drive error\n");
	/* flush cache */
	outb(PRIMARY + REG_CMD_STAT, CMD_FLUSH_CACHE);
}

static void primary_irq(struct int_regs *r)
{
	print("ata: irq 14\n");
}

static void secondary_irq(struct int_regs *r)
{
	print("ata: irq 15\n");
}

void init_hd()
{
	unsigned char c;
	unsigned char buf[2*512];
	int i;

	/* check for floating bus */
	c = inb(PRIMARY + REG_CMD_STAT);
	if (c == 0xff)
		print("ata primary: no drives\n");
	c = inb(PRIMARY + REG_CMD_STAT);
	if (c == 0xff)
		print("ata secondary: no drives\n");
	/* primary: IRQ 14, secondary: IRQ 15 */
	reg_interrupt_handler(32 + 14, &primary_irq);
	reg_interrupt_handler(32 + 15, &secondary_irq);
	enable_irq(14);
	enable_irq(15);
	/* drive ready? */
	do {
		c = inb(PRIMARY_CTRL);
		if ((c & STAT_ERR) || (c & STAT_DF))
			break;
	} while ((c & STAT_BSY) && !(c & STAT_DRQ));
	if ((c & STAT_ERR) || (c & STAT_DF))
		print("ata: drive error\n");
	/* select master on primary bus */
	outb(PRIMARY + REG_DRIVE, MASTER);
	/* delay 400 ns */
	(void)inb(PRIMARY_CTRL);
	(void)inb(PRIMARY_CTRL);
	(void)inb(PRIMARY_CTRL);
	(void)inb(PRIMARY_CTRL);
	/* drive ready? */
	do {
		c = inb(PRIMARY_CTRL);
		if ((c & STAT_ERR) || (c & STAT_DF))
			break;
	} while ((c & STAT_BSY) && !(c & STAT_DRQ));
	if ((c & STAT_ERR) || (c & STAT_DF))
		print("ata: drive error\n");
	/* disable interrupts */
	outb(PRIMARY_CTRL, CTRL_NIEN);
	/* test */
	read_lba48(0, 0, 1, buf);
	/*for (i = 0; i < 512; ++i)
		printf("%x ", buf[i]);*/
	/*for (i = 0; i < 2*512; ++i)
		buf[i] = (unsigned char)i;
	write_lba48(1, 0, 2, buf);*/
}
