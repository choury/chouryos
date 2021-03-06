;mem map  0--------0x7ff   idt
;         0x800----0xfff   gdt
;         0x1000---0x11ff  floopy buffer
;         0x1200---0x1267  tss
;         0x2800---0x2bff  real interupt handler table
;         0x2c00---0x2dff  tmp map bit mask
;         0x3000---0x3fff  a page for tmp map
;         0x10000--------  tmpmalloc
;         0x9f000-0xfffff  bios rom     0xB8000       console buffer
;         1M    ---------       
;               process table
;         2M    ---------       kernel(process0) code & date
;               ↓       ↓
;               .       .
;               ↑       ↑       init's stack
;         3M    ---------
;               memory map(1M)
;         4M    --------- 
;               内存共享链
;         5M    ---------

%include "asm.h"
%include "../boot/pm.h"
    extern Init8259
    extern init
    extern setinterrupt
    extern getmemmap
    global start
    global movetouse
    global do_switch_to
    
    
; setting up the Multiboot header - see GRUB docs for details

MODULEALIGN equ  1<<0                                   ; align loaded modules on page boundaries
MEMINFO     equ  1<<1                                   ; provide memory map
VIDINFO     equ  1<<2
FLAGS       equ  MODULEALIGN | MEMINFO | VIDINFO        ; this is the Multiboot 'flag' field
MAGIC       equ   0x1BADB002                            ; 'magic number' lets bootloader find the header
CHECKSUM    equ -(MAGIC + FLAGS)                        ; checksum required
    

use32
section .boothead
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM
    dd 0, 0, 0, 0, 0
    dd 1, 0, 0, 0

section .text
start:
    cli
    mov esp, 0x300000
    cmp eax, 0x2BADB002
    jne wrongboot
    push ebx
    call getmemmap
    add esp, 4
    mov esi,LABEL_GDT
    mov edi,2048
    mov ecx,2048
    cld
    rep movsb
    lgdt    [GdtPtr]
    lidt    [IdtPtr]
    jmp    KCODE_DT:next
next:
    mov ax,KDATA_DT
    mov ds,ax
    mov ss,ax
    mov es,ax
    mov gs,ax
    call init
wrongboot:
    jmp $




movetouse:
    push ebp
    mov ebp,esp
    mov eax, [ebp+12]
    mov cr3, eax
    mov eax, cr0
    or  eax, 0x80000000
    mov cr0, eax

    mov ax, TSS_DT
    ltr  ax
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

    mov eax, [es:ebp+16]
    mov ecx, [es:ebp+12]
    shl eax, 12
    mov cr3, eax
    mov esp, ecx
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
    
section .data
LABEL_GDT:          Descriptor  0,                  0,       0
LABEL_DESC_CODE32:  Descriptor  0,                  0xfffff, DA_C   +DA_32+ DA_G
LABEL_DESC_DATA:    Descriptor  0,                  0xfffff, DA_DRW +DA_32+ DA_G
LABEL_DESC_VGA:     Descriptor  0,                  0x160,   DA_DRW +DA_32+ DA_G + DA_DPL3

GdtLen equ  $-LABEL_GDT
GdtPtr: dw  2048               ;GDT Limit
        dd  2048               ;GDT Base


IdtPtr: dw  2048
        dd  0