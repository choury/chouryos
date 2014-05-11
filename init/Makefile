.PHONY: all clean

all:init echo

#好不爽啊，为什么这货链接出来有2M? ，我用的软盘一个才1.44M

init: main.o crt0.o
	$(LD) -o $@ $^ -Texe.ld

echo:echo.o crt0.o
	$(LD) -o $@ $^ -Texe.ld


clean:
	rm *.o init echo
