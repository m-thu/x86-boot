# boot loader

.set KERNEL_SIZE, 50			# number of 512 byte blocks
.set KERNEL_ADDRESS, 0x7e00		# load kernel to this address

.text
.code16
.globl start

start:
	ljmp $0, $real_start		# cs = 0
real_start:
	cli
	xorw %ax, %ax			# setup stack and segments
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss
	movw $0x7c00, %sp
	sti

	pushw %dx			# save bootdrive (dl)

	call a20_enable			# enable A20
	pushw $boot			# print message
	call print_string

	movb $0x3, %ah			# get cursor position
	xorb %bh, %bh			# page
	int $0x10
	movw %dx, cursor_position	# dh: row, dl: column

	popw %dx			# get bootdrive
	movb $0x42, %ah			# extended read
	movw $dap, %si			# ds:si disk address packet
	int $0x13
	jnc 2f				# sets carry on error
	pushw $read_error		# print error message
	call print_string
1:	hlt				# hang
	jmp 1b

2:
	cli
	lgdt gdt_pointer		# load temporary GDT
	movl %cr0, %eax
	orb $1, %al
	movl %eax, %cr0			# enable protected mode

	ljmp $0b1000, $pmode		# ring0, gdt, first gdt entry

.code32
pmode:
	movw $0b10000, %ax		# ring0, gdt, second entry
	movw %ax, %ds			# update segment registers
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss

	ljmp $0b1000, $KERNEL_ADDRESS	# jump to 'start'

.code16

# enable a20 gate
a20_enable:
	call a20_wait
	movb $0xd1, %al			# write command
	outb %al, $0x64
	call a20_wait
	movb $0xdf, %al			# enable a20
	#movb $0xdd, %al		# disable a20
	outb %al, $0x60
	call a20_wait

	ret

# wait for keyboard controller
a20_wait:
	inb $0x64, %al
	testb $2, %al
	jnz a20_wait

	ret

# print string
# parameter: string offset
print_string:
	pushw %bp
	movw %sp, %bp

	movw 4(%bp), %si
	cld
1:
	lodsb
	testb %al, %al
	jz 2f
	movb $0xe, %ah			# write character
	movw $0x7, %bx			# bh: page, bl: foreground color
	int $0x10			# character: al
	jmp 1b

2:
	popw %bp
	ret $2

# DAP (disk address packet, http://www.ctyme.com/intr/rb-0708.htm)
dap:
dap_size:	.byte 0x10			# size of structure
dap_reserved:	.byte 0x00
dap_blocks:	.word KERNEL_SIZE		# number of blocks
dap_buffer:	.word KERNEL_ADDRESS, 0x00	# seg:offset of buffer
dap_lba:	.long 1				# lba address

# GDT
gdt:

# first entry is always zero
	.long 0,0

# code segment
	.word 0xffff		# limit (lower 16 bits)
	.word 0x0000		# base (lower 16 bits)
	.byte 0x00		# base (next 8 bits)
	.byte 0b10011010	# access byte
#	present; ring0; code or data segment; code(=1), execute/read,
#	not accessed
	.byte 0b11001111	# granularity byte
#	granularity, 32 bit, always zero, always zero, limit (last 4 bits)
	.byte 0x00		# base (last 8 bits)

# data segment
	.word 0xffff		# limit (lower 16 bits)
	.word 0x0000		# base (lower 16 bits)
	.byte 0x00		# base (next 8 bits)
	.byte 0b10010010	# access byte
#       present; ring0; code or data segment; data(=0), read/write,
#	not accessed
	.byte 0b11001111	# granularity byte
#       granularity, 32 bit, always zero, always zero, limit (last 4 bits)
	.byte 0x00		# base (last 8 bits)

gdt_end:

# pointer to GDT
gdt_pointer:
	.word gdt_end - gdt - 1	# size of GDT-1
	.long gdt		# GDT offset

# strings
boot:		.asciz "Loading kernel...\r\n"
read_error:	.asciz "Read error!"

. = start + 510
cursor_position:		# cursor position for later use
.word 0xaa55			# boot signature
