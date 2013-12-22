IDT_START equ 0
GDT_START equ 2048

INTHER    equ 10240

;以下定义在chouryos.h中
KERNELCODE_DT equ 8 
KERNELDATA_DT equ 16
VGA_DT        equ 24
TSS_DT        equ 32
LDT_START     equ 40
L_KSDT        equ 20
KSL           equ 0x1000
PROTABLE      equ 0x100000  ;定义于 process.h


