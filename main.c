#include "print.h"
#include "gdt.h"
#include "idt.h"
#include "isr.h"
#include "exception.h"
#include "keyboard.h"
#include "timer.h"
#include "paging.h"
#include "cpuid.h"
#include "string.h"
#include "rtc.h"
#include "ata.h"

void kmain()
{
	unsigned int cr0;
	struct time *time;

	/* clear screen */
	init_cursor();
	/*cls();*/
	print("Hello world!\n");

	asm volatile ("movl %%cr0, %0" : "=r"(cr0));
	printf("CR0: %b\n", cr0);

	/* init GDT */
	print("Loading GDT... ");
	init_gdt();
	print("done\n");

	/* init IDT */
	print("Reprogramming PIC and loading IDT... ");
	init_idt();
	print("done\n");

	/* set interrupt handlers for exceptions */
	reg_interrupt_handler(0, &ex_div_by_zero);
	reg_interrupt_handler(13, &ex_general_protection_fault);

	print("Setting up keyboard... ");
	/* init keyboard, set handler and unmask IRQ1 */
	init_keyboard();
	print("done\n");

	print("Setting up PIT @50Hz... ");
	init_timer(50);
	print("done\n");

	/*asm volatile ("int $3");*/

	/*print("Setting up paging... ");
	init_paging();
	print("done\n");*/

	asm volatile ("movl %%cr0, %0" : "=r"(cr0));
	printf("CR0: %b\n", cr0);

	cpuid();

	/* init realtime clock */
	print("Setting up realtime clock... ");
	init_rtc();
	print("done\n");

	print("Init HD... ");
	init_hd();
	print("done\n");
}
