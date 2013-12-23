.PHONY: all clean

all:chouryos

chouryos:../asm/kernel.o ../asm/Syscall.o ../asm/baseio.o ../asm/interrupt.o\
	init.o syscall.o floppy.o hd.o keyboad.o fat.o file.o graphy.o vsprintf.o putstring.o\
	open.o read.o write.o close.o lseek.o fstat.o sbrk.o fork.o times.o execve.o schedule.o
	$(LD) -o $@.elf $^  -Tkernel.ld -m elf_i386
	$(OBJCOPY) -R .pdr -R .comment -R .note -I elf32-i386 -O binary $@.elf $@

clean:
	rm *.o chouryos chouryos.elf
