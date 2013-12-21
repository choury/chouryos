export CC=gcc
export LD=ld
export AS=nasm
export AR=ar
export OBJCOPY=objcopy


CWD = $(shell pwd)
export ROOT=$(CWD)
export CPPFLAGS=-m32 -Wall -fno-leading-underscore -fno-builtin -I$(ROOT)/include -I$(ROOT)/newlib-i386/include


.PHONY : all kernel boot asm exe clean

all: boot asm kernel exe

boot:kernel
	$(MAKE) -C boot


boot.img:
	@dd if=boot/boot of=boot.img bs=512 count=1
	@dd if=/dev/zero of=boot.img skip=1 seek=1 bs=512 count=2879



install:boot kernel exe
#	sudo mount -o umask=000 /dev/sdb1 /mnt
#	cp boot/loader /mnt
#	cp kernel/chouryos /mnt
#	sudo umount /mnt
	sudo mount -o loop,offset=1048576,umask=000 hd.img /mnt
	cp boot/loader /mnt
	cp kernel/chouryos /mnt
	cp exe/exe.elf /mnt
	sudo umount /mnt
#	sync

kernel:asm
	$(MAKE) -C kernel

asm:
	$(MAKE) -C asm

exe:asm
	$(MAKE) -C exe


clean:
	$(MAKE) clean -C boot
	$(MAKE) clean -C kernel
	$(MAKE) clean -C asm
	$(MAKE) clean -C exe
	@rm -f *.o *.elf loader chouryos
