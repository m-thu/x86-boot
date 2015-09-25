SRC:=main.c print.c gdt.c idt.c isr.c exception.c keyboard.c timer.c \
	cpuid.c rtc.c ata.c
SRC_ASM:=interrupt.s
GCC_WARNINGS:=-Wall -Wextra #-pedantic
OPTFLAGS:=-Os #-fomit-frame-pointer
CFLAGS:=-nostdlib -nostdinc -fno-builtin -fno-stack-protector $(GCC_WARNINGS) \
	$(OPTFLAGS) $(CC_DEBUG_FLAGS) -pipe -m32 -march=i686
TARGET:=i686-elf
PREFIX:=~/$(TARGET)/bin

CC:=$(PREFIX)/$(TARGET)-gcc
AS:=$(PREFIX)/$(TARGET)-as
LD:=$(PREFIX)/$(TARGET)-ld

OBJ:=$(SRC:.c=.o)
OBJ_ASM:=$(SRC_ASM:.s=.o)

all: boot.bin link

print.c: print.h colors.h
gdt.c: gdt.h
idt.c: idt.h
isr.c: isr.h
exception.c: exception.h
keyboard.c: keyboard.h
timer.c: timer.h
asm.asm: asm.h
cpuid.c: cpuid.h
rtc.c: rtc.h
ata.c: ata.h

boot.bin: boot.s
	$(AS) -o boot.o boot.s
	$(LD) -T link_boot.ld -o boot.bin boot.o

%.o: %.s
	$(AS) $(AS_DEBUG_FLAGS) -o $@ $<

link: start.o $(OBJ) $(OBJ_ASM)
	$(LD) -T link.ld -o kernel.bin start.o $(OBJ) $(OBJ_ASM)

image: clean all
	cc -o count count.c
	dd if=/dev/zero of=blank bs=1 count=`./count`
	cat boot.bin kernel.bin blank >image
	rm -f blank

debug: clean start.o $(OBJ) $(OBJ_ASM)
	$(LD) -T link_debug.ld -o kernel_debug start.o $(OBJ) $(OBJ_ASM)

clean:
	rm -f *.bin *.o
clean-all: clean
	rm -f image kernel_debug count

.PHONY: all link clean image debug clean-all
