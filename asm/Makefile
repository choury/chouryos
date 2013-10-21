all:kernel.o Syscall.o crt0.o



kernel.o:kernel.asm
	$(AS) $< -f elf32 -o $@

Syscall.o:syscall.asm
	$(AS) $< -f elf32 -o $@

crt0.o:crt0.asm
	$(AS) $< -f elf32 -o $@

clean:
	rm *.o
