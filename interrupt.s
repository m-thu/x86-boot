# void isr_handler(struct int_regs): isr.c
# void irq_handler(struct int_regs): isr.c
.text
.code32

# parameter: interrupt
.macro ISR_NOERRORCODE int
	.globl isr\int

	isr\int\():
		cli			# disable interrupts
		pushl $0		# push dummy error code
		pushl $\int		# push interrupt number
		jmp common_isr_handler
.endm

# parameter: interrupt
.macro ISR_ERRORCODE int
	.globl isr\int

	isr\int\():
		cli			# disable interrupts
		pushl $\int		# push interrupt number
		jmp common_isr_handler
.endm

# parameter: irq
.macro IRQ int
	.globl irq\int
	irq\int\():
		cli
		pushl $0		# push dummy error code
		pushl $\int		# push irq
		jmp common_irq_handler
.endm

# handler for interrupts 0-31
common_isr_handler:
	pusha				# eax,ecx,edx,ebx,esp,ebp,esi,edi
	pushl %ds			# save segments
	pushl %es
	pushl %fs
	pushl %gs

	movw $0b10000, %ax		# kernel data segment
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs

	call isr_handler

	popl %gs
	popl %fs
	popl %es
	popl %ds
	popa
	addl $(2*4), %esp		# remove int number and error code
	sti				# enable interrupts
	iret

# handler for irqs 0-15
common_irq_handler:
	pusha				# eax,ecx,edx,ebx,esp,ebp,esi,edi
	pushl %ds			# save segments
	pushl %es
	pushl %fs
	pushl %gs

	movw $0b10000, %ax		# kernel data segment
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs

	call irq_handler

	popl %gs
	popl %fs
	popl %es
	popl %ds
	popa
	addl $(2*4), %esp		# remove int number and error code
	sti				# enable interrupts
	iret

# interrupts with error code: 8, 10-14
ISR_NOERRORCODE 0
ISR_NOERRORCODE 1
ISR_NOERRORCODE 2
ISR_NOERRORCODE 3
ISR_NOERRORCODE 4
ISR_NOERRORCODE 5
ISR_NOERRORCODE 6
ISR_NOERRORCODE 7
ISR_ERRORCODE 8
ISR_NOERRORCODE 9
ISR_ERRORCODE 10
ISR_ERRORCODE 11
ISR_ERRORCODE 12
ISR_ERRORCODE 13
ISR_ERRORCODE 14
ISR_NOERRORCODE 15
ISR_NOERRORCODE 16
ISR_NOERRORCODE 17
ISR_NOERRORCODE 18
ISR_NOERRORCODE 19
ISR_NOERRORCODE 20
ISR_NOERRORCODE 21
ISR_NOERRORCODE 22
ISR_NOERRORCODE 23
ISR_NOERRORCODE 24
ISR_NOERRORCODE 25
ISR_NOERRORCODE 26
ISR_NOERRORCODE 27
ISR_NOERRORCODE 28
ISR_NOERRORCODE 29
ISR_NOERRORCODE 30
ISR_NOERRORCODE 31

# IRQs
IRQ 0
IRQ 1
IRQ 2
IRQ 3
IRQ 4
IRQ 5
IRQ 6
IRQ 7
IRQ 8
IRQ 9
IRQ 10
IRQ 11
IRQ 12
IRQ 13
IRQ 14
IRQ 15
