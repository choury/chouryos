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
    global do_switch_to
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
    mov esp, [ebp+0x8]
    pop gs
    pop fs
    pop es
    pop ds
    popad
    iret

do_switch_to:
    push ebp
    mov ebp, esp
    mov dx, es                              ;保存原es
    mov ax, ss                              ;原ss存到es
    mov es, ax
    mov ax, ds                              ;ss改为ds的值，即KERNEL_DATA
    mov ss, ax
    mov esp, [es:ebp+0x8]
    push es                                 ;这两个参数是没有意义的，不会被恢复
    push ebp                                ;
    pushf
    push cs
    push back
    pushad
    push ds
    push es
    push fs
    push gs
    
    mov esp, [es:ebp+12]
    lldt [es:ebp+16]
    pop gs
    pop fs
    pop es
    pop ds
    popad
    iret
back:
    mov ax, es                              ;将ss恢复为es内容
    mov ss, ax
    mov es, dx                              ;恢复es内容
    leave
    ret