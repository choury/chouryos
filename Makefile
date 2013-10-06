export CC=gcc
export LD=ld
export AS=nasm
export AR=ar
export OBJCOPY=objcopy


CWD = $(shell pwd)
export ROOT=$(CWD)
export CPPFLAGS=-m32 -Wall -fno-leading-underscore -I$(ROOT)/include -I$(ROOT)/newlib-i386/include


.PHONY : all kernel boot asm clean

all: boot.img

boot:
	$(MAKE) -C boot


boot.img:
	@dd if=boot/boot of=boot.img bs=512 count=1
	@dd if=/dev/zero of=boot.img skip=1 seek=1 bs=512 count=2879


	
copy:boot kernel boot.img
	sudo mount -o loop,umask=000 boot.img /mnt
	cp boot/loader /mnt
	cp kernel/chouryos /mnt
	sudo umount /mnt

kernel:asm
	$(MAKE) -C kernel

asm:
	$(MAKE) -C asm	
	
clean:
	$(MAKE) clean -C boot
	$(MAKE) clean -C kernel
	$(MAKE) clean -C asm
	@rm -f *.o *.elf loader chouryos

cleanall:clean
	@rm boot.img
