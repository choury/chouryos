VPATH = $(BUILDDIR)

CFLAGS += -D __BOOT__


all:boot loader


boot:boot.asm
	$(AS) $< -f bin -o $@

loader.o: loader.asm
	$(AS) $(ASFLAGS) $< -o $@

interrupt.o: interrupt.asm
	$(AS) $(ASFLAGS) $< -o $@

	
loader: loader.o baseio.o interrupt.o loadkernel.o fat.o file.o floppy.o hd.o putstring.o times.o vsprintf.o
	$(LD) -o $@.elf $^ -Tloader.ld -melf_i386
	$(OBJCOPY) -R .pdr -R .comment -R .note -I elf32-i386 -O binary $@.elf $@
	
clean:
	rm *.o boot loader loader.elf
