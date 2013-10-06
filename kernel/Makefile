.PHONY: all clean

all:chouryos

chouryos: init.o syscall.o floppy.o keyboad.o fat.o file.o graphy.o open.o read.o write.o close.o fstat.o
	$(LD) -o $@.elf ../asm/kernel.o ../asm/syscall.o $^ ../newlib-i386/libc.a -Tkernel.ld -melf_i386
	$(OBJCOPY) -R .pdr -R .comment -R .note -I elf32-i386 -O binary $@.elf $@

clean:
	rm *.o chouryos chouryos.elf
