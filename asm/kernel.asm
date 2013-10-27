;mem map  0---0x7ff      idt
;         0x800---0xfff   gdt
;         0x1000---0x11ff   floopy buffer
;         0x1200---0x1267   tss
;         0x1300---0x1500  hdd info
;         0x2800--0x2c00   realinthandler table
;         0xa0000-0xfffff  bios rom     0xB8000       console buffer
;         1M------         process table
;         2M    ---------       kernel code & date
;               ↓       ↓
;               .
;               ↑       ↑       kernel stack
;         3M    ---------
;               ↓       ↓       kernel heap
;               .
;               ↑       ↑       process0 stack
;         4M    ---------

%include "asm.h"


    extern init
    extern setinterrupt
    global movetouse
use32
start:
    cli
    mov ax,KERNELDATA_DT
    mov ds,ax
    mov ss,ax
    mov es,ax
    mov gs,ax
    mov esp,0x1ffffe
    call setinterrupt
    call init
    jmp $




movetouse:
    push ebp
    mov ebp,esp
    mov ax, TSS_DT
    ltr  ax
    mov ax, LDT_START
    lldt ax
    dec dword [reenter]
    mov esp, [ebp+0x8]
    pop gs
    pop fs
    pop es
    pop ds
    popad
    iret

