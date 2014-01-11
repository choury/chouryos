ENTRY(start)


SECTIONS
{
  . = 0x200000;
  .boothead : { *(.boothead) }
  .text : { *(.text) }
  .data : { *(.data) }
  .bss : { *(.bss) }
}
