#include "isr.h"
#include "print.h"

static void dump_registers(struct int_regs *r)
{
	printf("cs:eip: 0x%x:0x%x\n", r->cs, r->eip);
	printf("flags: 0x%x\n", r->eflags);
	printf("eax: 0x%x  ebx: 0x%x  ecx: 0x%x\n", r->eax, r->ebx, r->ecx);
	printf("edx: 0x%x  esi: 0x%x  edi: 0x%x\n", r->edx, r->esi, r->edi);
}

void ex_div_by_zero(struct int_regs *r)
{
	print("\n!!!!!!!!!!!!!!!!!!!!!!!\n");
	print("PANIC: division by zero\n");
	print("!!!!!!!!!!!!!!!!!!!!!!!\n");
	dump_registers(r);
	for (;;)
		asm volatile ("hlt");
}

void ex_general_protection_fault(struct int_regs *r)
{
	print("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	print("PANIC: general protection fault\n");
	print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	printf("error code: 0x%x\n", r->error_code);
	dump_registers(r);
	for (;;)
		asm volatile ("hlt");
}
