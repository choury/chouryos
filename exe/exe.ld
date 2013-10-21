ENTRY(start)
SEARCH_DIR(../asm)
SEARCH_DIR(../newlib-i386)
STARTUP(crt0.o)
INPUT(Syscall.o)
INPUT(libc.a)
OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)

SECTIONS
{
. = 0x00000;
.text : { *(.text) }
.data : { *(.data) }
.bss : { *(.bss) }
}
