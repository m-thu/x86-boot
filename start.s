.text
.code32
.globl start

start:
	movl $0x9fbff, %esp		# setup 32 bit stack
	call kmain			# call kernel main
1:
	hlt
	jmp 1b
