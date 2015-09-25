#ifndef ATA_H
#define ATA_H

void init_hd();
void read_lba48(unsigned int, unsigned int, unsigned short, unsigned char *);
void write_lba48(unsigned int, unsigned int, unsigned short, unsigned char *);

/* primary and secondary bus io ports */
#define PRIMARY		0x1f0
#define PRIMARY_CTRL	0x3f6		/* control/alternate status */
#define SECONDARY	0x170
#define SECONDARY_CTRL	0x376

/* registers */
#define REG_DATA	0		/* data read/write */
#define REG_ERROR	1		/* features/error (atapi) */
#define REG_SEC_COUNT	2		/* number of sectors */
#define REG_SEC_NUM	3		/* sector num./ LBA low */
#define REG_CYL_LOW	4		/* cyl. low/ LBA mid */
#define REG_CYL_HIGH	5		/* cyl. high/ LBA high */
#define REG_DRIVE	6		/* select drive/head */
#define REG_CMD_STAT	7		/* command/status */

/* status byte */
#define STAT_ERR	(1 << 0)	/* set if error */
#define STAT_DRQ	(1 << 3)	/* pio data ready to read/write */
#define STAT_SRV	(1 << 4)	/* overlapped mode service request */
#define STAT_DF		(1 << 5)	/* drive fault error */
#define STAT_RDY	(1 << 6)	/* set if ready */
#define STAT_BSY	(1 << 7)	/* set if busy */

/* control register */
#define CTRL_NIEN	(1 << 1)	/* no interrupts if set */
#define CTRL_SRST	(1 << 2)	/* software reset if set */
#define CTRL_HOB	(1 << 7)	/* read back lba48 high order byte */

/* drive select */
#define MASTER		0xa0
#define SLAVE		0xb0

/* commands */
#define CMD_FLUSH_CACHE	0xe7		/* flush cache after write */
#define CMD_IDENTIFY	0xec		/* identify command */
#define CMD_WRITE_EXT	0x34		/* LBA48 write */
#define CMD_READ_EXT	0x24		/* LBA48 read */

#endif
