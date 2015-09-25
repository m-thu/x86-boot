#ifndef EXCEPTION_H
#define EXCEPTION_H

void ex_div_by_zero(struct int_regs *);
void ex_general_protection_fault(struct int_regs *);

#endif
