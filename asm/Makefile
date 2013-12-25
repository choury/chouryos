all:kernel.o syscall.o crt0.o baseio.o interrupt.o



kernel.o:kernel.asm
	$(AS) $(ASFLAGS) $< -o $@

syscall.o:syscall.asm
	$(AS) $(ASFLAGS) $< -o $@

baseio.o: baseio.asm
	$(AS) $(ASFLAGS) $< -o $@

crt0.o:crt0.asm
	$(AS) $(ASFLAGS) $< -o $@

interrupt.o:interrupt.asm
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm *.o
