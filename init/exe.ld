ENTRY(start)
SEARCH_DIR(../build)
SEARCH_DIR(../newlib)
INPUT(libsyscall.o)
INPUT(libc.a)
OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)

SECTIONS
{
. = 0x40001000;
.text : { *(.text) }
.data : { *(.data) }
.bss : { *(.bss) }
}
