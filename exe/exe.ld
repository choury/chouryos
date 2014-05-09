ENTRY(start)
SEARCH_DIR(../build)

INPUT(libsyscall.o)
OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)

SECTIONS
{
. = 0x40001000;
.text : { *(.text) }
.data : { *(.data) }
.bss : { *(.bss) }
}
