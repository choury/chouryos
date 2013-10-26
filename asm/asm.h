IDT_START equ 0
GDT_START equ 2048

INTHER    equ 10240

KERNELCODE_DT equ 8
KERNELDATA_DT equ 16
VGA_DT        equ 24
TSS_DT        equ 32
LDT_START     equ 40


extern  reenter
extern  stacktop
