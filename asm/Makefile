all:kernel.o syscall.o



kernel.o:kernel.asm
	$(AS) $< -f elf32 -o $@

syscall.o:syscall.asm
	$(AS) $< -f elf32 -o $@
clean:
	rm *.o
