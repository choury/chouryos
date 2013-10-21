all:boot loader


boot:boot.asm
	$(AS) $< -f bin -o $@

loader.o: loader.asm
	$(AS) $< -f elf32 -o $@

kernel.o:kernel.asm
	$(AS) $< -f elf32 -o $@

loadkernel.o:loadkernel.asm
	$(AS) $< -f elf32 -o $@

interrupt.o: interrupt.asm
	$(AS) $< -f elf32 -o $@

baseio.o: baseio.asm	
	$(AS) $< -f elf32 -o $@

	
loader: loader.o interrupt.o baseio.o floppy.o file.o fat.o loadkernel.o fat.o file.o floppy.o vsprintf.o
	$(LD) -o $@.elf $^ -Tloader.ld -melf_i386
	$(OBJCOPY) -R .pdr -R .comment -R .note -I elf32-i386 -O binary $@.elf $@
	
clean:
	rm *.o boot loader loader.elf
