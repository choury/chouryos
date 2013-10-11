.PHONY: all clean

all:chouryos

chouryos:../asm/kernel.o ../asm/syscall.o init.o syscall.o floppy.o keyboad.o fat.o file.o graphy.o \
	open.o read.o write.o close.o fstat.o sbrk.o fork.o
	$(LD) -o $@.elf $^ ../newlib-i386/libc.a -Tkernel.ld -melf_i386
	$(OBJCOPY) -R .pdr -R .comment -R .note -I elf32-i386 -O binary $@.elf $@

clean:
	rm *.o chouryos chouryos.elf
