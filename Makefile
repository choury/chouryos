KNAME=chouryos

export CC=gcc
export LD=ld
export AS=nasm
export AR=ar
export OBJCOPY=objcopy


CWD = $(shell pwd)
export ROOT=$(CWD)
export CPPFLAGS=-m32 -g -c -Wall -fno-leading-underscore -fno-builtin -I$(ROOT)/include -I$(ROOT)/newlib/include
export ASFLAGS=-w+orphan-labels -f elf32 -g
export BUILDDIR=$(ROOT)/build

.PHONY : all kernel boot asm init clean

all:asm kernel init #boot
	$(LD) -o $(KNAME).elf $(wildcard $(BUILDDIR)/*.o)  -Tkernel.ld -m elf_i386
#	$(OBJCOPY) -R .pdr -R .comment -R .note -I elf32-i386 -O binary $(KNAME).elf $(KNAME)

boot:kernel
	$(MAKE) -C boot


boot.img:
	@dd if=boot/boot of=boot.img bs=512 count=1
	@dd if=/dev/zero of=boot.img skip=1 seek=1 bs=512 count=2879



install:all
#	sudo mount -o umask=000 /dev/sdb1 /mnt
#	cp boot/loader /mnt
#	cp kernel/chouryos /mnt
#	sudo umount /mnt
	sudo mount -o loop,offset=1048576,umask=000 $(BUILDDIR)/hd.img /mnt
#	cp boot/loader /mnt
#	cp $(KNAME) /mnt
	cp $(KNAME).elf /mnt
	cp init/init /mnt
	cp init/echo /mnt
	sync
	sudo umount /mnt

kernel:asm
	$(MAKE) -C kernel

asm:
	$(MAKE) -C asm

init:asm
	$(MAKE) -C init


clean:
	@rm -f $(BUILDDIR)/*.o
#	$(MAKE) clean -C boot
	$(MAKE) clean -C init
	@rm -f $(KNAME) $(KNAME).elf
