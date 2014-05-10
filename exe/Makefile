.PHONY: all clean

all:exe.elf

#好不爽啊，为什么这货链接出来有2M? ，我用的软盘一个才1.44M

exe.elf: main.o crt0.o
	$(LD) -o $@ $^ -Texe.ld

#crt0.o:crt0.asm
#	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm *.o exe.elf
