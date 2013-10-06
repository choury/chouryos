# chapter2/1/Makefile
#
# Author: Wenbo Yang <solrex@gmail.com> <http://solrex.cn>
#
# This file is part of the source code of book "Write Your Own OS with Free
# and Open Source Software". Homepage @ <http://share.solrex.cn/WriteOS/>.
#
# This file is licensed under the GNU General Public License; either
# version 3 of the License, or (at your option) any later version.

CC=gcc -c -g0 -m32
LD=ld
AS=nasm
BOOTLDFILE=boot.ld
LOADLDFILE=loader.ld
TESRLDFILE=test.ld
OBJCOPY=objcopy

all: boot.img

boot.img: boot
	@dd if=boot of=boot.img bs=512 count=1
	@dd if=/dev/zero of=boot.img skip=1 seek=1 bs=512 count=2879

boot.bin: boot.elf
	@$(OBJCOPY) -R .pdr -R .comment -R.note -S -O binary boot.elf boot.bin

boot.elf: boot.o
	$(LD) boot.o -o boot.elf -T$(BOOTLDFILE) -melf_i386

boot.o: boot.S
	$(CC) $< -o $@

boot:boot.asm
	$(AS) $< -f bin -o $@
	
main.o: main.c
	$(CC) $< -o $@

disk.o: disk.asm
	$(AS) $< -f elf32 -o $@

loader.o: loader.asm
	$(AS) $< -f elf32 -o $@

baseio.o: baseio.asm
	$(AS) $< -f elf32 -o $@

interrupt.o: interrupt.asm
	$(AS) $< -f elf32 -o $@	
	
fat.o:fat.c
	$(CC) $< -o $@
	
graphy.o:graphy.c
	$(CC) $< -o $@
	
vsprintf.o:vsprintf.c
	$(CC) $< -o $@

file.o:file.c
	$(CC) $< -o $@

floppy.o:floppy.c	
	$(CC) $< -o $@ 

test.o:test.c	
	$(CC) $< -o $@	
	
exec:exec.asm
	$(AS) $< -f bin -o $@

process.o:process.asm
	$(AS) $< -f elf32 -o $@
	
loader: loader.o interrupt.o baseio.o main.o vsprintf.o graphy.o floppy.o file.o fat.o process.o
	$(LD) -o $@.elf $^ -T$(LOADLDFILE) -melf_i386
	$(OBJCOPY) -R .pdr -R .comment -R .note -I elf32-i386 -O binary $@.elf $@

test:   test.o	vsprintf.o baseio.o interrupt.o
	$(LD) -o $@.elf $^ -Ttest.ld -melf_i386
	$(OBJCOPY) -R .pdr -R .comment -R .note -I elf32-i386 -O binary $@.elf $@
	
copy: boot.img loader exec test
	kdesudo "mount -o loop,umask=000 boot.img /mnt"
	cp loader /mnt
	cp exec   /mnt
	cp test   /mnt
	kdesudo "umount /mnt"

clean:
	@rm -f *.o *.elf boot.bin *~ boot.img loader test