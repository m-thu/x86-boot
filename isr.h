#ifndef ISR_H
#define ISR_H

/* stack layout before call to isr_handler/irq_handler */
struct int_regs {
	unsigned int gs, fs, es, ds;		/* saved segment registers*/
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;	/* pusha */
	unsigned int interrupt, error_code;
	unsigned int eip, cs, eflags, original_esp, ss;		/* interrupt */
} __attribute__((packed));

/* function pointer for interrupt handler */
typedef void (*interrupt_handler)(struct int_regs *);

void init_interrupt_handlers();
void reg_interrupt_handler(unsigned char, interrupt_handler);
void enable_irq(unsigned char);
void disable_irq(unsigned char);

#endif
