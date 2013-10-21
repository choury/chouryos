SEARCH_DIR(../asm)
SEARCH_DIR(../newlib-i386)
STARTUP(kernel.o)
INPUT(Syscall.o)

OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)

SECTIONS
{
  . = 0x200000;
  .text :
  {
    _ftext = .;
  } = 0
}
