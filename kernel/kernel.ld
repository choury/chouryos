SEARCH_DIR(../asm)
SEARCH_DIR(../newlib-i386)
STARTUP(kernel.o)
INPUT(Syscall.o)
INPUT(baseio.o)
INPUT(interrupt.o)

OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)

SECTIONS
{
  . = 0x200000;
  .text : { *(.text) }
  .data : { *(.data) }
  .bss : { *(.bss) }
}
