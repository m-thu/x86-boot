/* http://www.nondot.org/sabre/os/files/HCI/keyboard2.txt */

#include "keyboard.h"
#include "isr.h"
#include "port.h"
#include "print.h"

/* size of keyboard buffer */
#define BUFFER_SIZE 20

/* wait for empty keyboard command buffer */
/*#define KBD_WAIT while (inb(0x64) & 2)*/
static void kbd_wait()
{
	while (inb(0x64) & 2);
}

/* keyboard shift, capslock, alt, altgr status, ctrl */
static unsigned char shift = 0, capslock = 0, alt = 0, altgr = 0, ctrl = 0;
/* LED status */
static unsigned char led_capslock = 0, led_numlock = 0, led_scroll = 0;
/* keyboard ring buffer */
static unsigned char buffer[BUFFER_SIZE];
static unsigned char head = 0, tail = 0;

static void add_buffer(unsigned char c)
{
	buffer[head] = c;
	++head;
	if (head == BUFFER_SIZE)
		head = 0;
	if (head == tail) {
		++tail;
		if (tail == BUFFER_SIZE)
			tail = 0;
	}
}

static unsigned char get_buffer()
{
	unsigned char tmp = buffer[tail];

	if (tail == head) {
		return 0;
	} else {
		++tail;
		if (tail == BUFFER_SIZE)
			tail = 0;
		return tmp;
	}
}

static void set_kbd_leds()
{
	kbd_wait();
	/* set leds command */
	outb(0x60, 0xed);
	kbd_wait();
	/* led status byte */
	outb(0x60, led_scroll | (led_numlock << 1) | (led_capslock << 2));
}

/* keyboard interrupt handler */
static void kbd_handler(struct int_regs *regs)
{
	unsigned char scancode;
	static unsigned char extended = 0;

	/* wait for data in keyboard buffer */
	while (!(inb(0x64) & 1));
	/* read scancode */
	scancode = inb(0x60);

	/* previous scancode was an extended scancode */
	if (extended) {
		extended = 0;
		printf("extended scancode: 0x%x  0b%b\n", scancode, scancode);
		switch (scancode) {
		case 0x38: /* altgr */
			altgr = 1;
			break;
		case 0xb8: /* altgr release */
			altgr = 0;
			break;
		case 0x1d: /* right ctrl */
			ctrl = 1;
			break;
		case 0x9d: /* right ctrl release */
			ctrl = 0;
			break;
		default:
			add_buffer(scancode);
			break;
		}
	} else {
		/*printf("dbg raw scancode: %x\n", scancode);*/
		switch (scancode) {
		case 0x00: /* keyboard error */
		case 0xff:
			print("keyboard debug: error\n");
			break;
		case 0xfa: /* ACK */
			/*print("keyboard debug: ACK\n");*/
			break;
		/*case 0xaa:*/ /* BAT end, also left shift release !! */
		case 0xee: /* echo */
		case 0xfc: /* BAT failed */
		case 0xfe: /* resend data */
			/* ignore */
			print("keyboard debug: ignore\n");
			break;
		case 0xe0: /* extended scancode */
			extended = 1;
			break;
		case 0x2a: /* left shift */
		case 0x36: /* right shift */
			shift = 1;
			break;
		case 0xaa: /* left shift release */
		case 0xb6: /* right shift release */
			shift = 0;
			break;
		case 0x3a: /* capslock */
			capslock = capslock?0:1;
			led_capslock = capslock;
			set_kbd_leds();
			break;
		case 0x38: /* alt */
			alt = 1;
			break;
		case 0xb8: /* alt release */
			alt = 0;
			break;
		case 0x1d: /* left ctrl */
			ctrl = 1;
			break;
		case 0x9d: /* left ctrl release */
			ctrl = 0;
			break;
		default:
			print("scancode: 0x");print_hex(scancode);
			print("  0b");print_bin(scancode);print("\n");
			add_buffer(scancode);
			break;
		}
	}
}

void init_keyboard()
{
	/* register handler for IRQ1 */
	reg_interrupt_handler(32 + 1, &kbd_handler);
	enable_irq(1);

	/* select scancode set 2 */
	kbd_wait();
	outb(0x60, 0xf0);
	kbd_wait();
	outb(0x60, 2);

	set_kbd_leds();
}
