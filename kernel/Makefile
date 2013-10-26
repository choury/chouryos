.PHONY: all clean

all:chouryos

chouryos:init.o syscall.o floppy.o hd.o keyboad.o fat.o file.o graphy.o vsprintf.o string.o lib.o\
	open.o read.o write.o close.o lseek.o fstat.o sbrk.o fork.o times.o execve.o
	$(LD) -o $@.elf $^  -Tkernel.ld
	$(OBJCOPY) -R .pdr -R .comment -R .note -I elf32-i386 -O binary $@.elf $@

clean:
	rm *.o chouryos chouryos.elf
