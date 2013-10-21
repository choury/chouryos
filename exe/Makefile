.PHONY: all clean

all:exe.elf

exe.elf: main.o
	$(LD) -o $@ $^ -lc -Texe.ld

clean:
	rm *.o exe.elf
