%include "pm.h"
;    extern  start
;    extern  putchar
    global  Init8259
    global  setinterrupt
    global  floppystatus
use32
section .text


Init8259:
    push eax
    mov al,11h
    out 20h,al   ;master 8259 icw1
    nop
    
    out 0a0h,al  ;slave 8259 icw1
    nop
    
    mov al,20h
    out 21h,al   ;master ,icw2
    nop
    
    mov al,28h
    out 0a1h,al  ;slave ,icw2
    nop
    
    mov al,04h   ;master ,icw3
    out 21h,al
    nop
    
    mov al,02h
    out 0a1h,al  ;slave,icw3
    nop
    
    mov al,01h
    out 21h,al   ;master,icw4
    nop
    
    out 0a1h,al  ;slave,icw4
    nop
    
    mov al,0bfh
    out 21h,al   ;maser,ocw1
    nop
    
    mov al,0ffh
    out 0a1h,al  ;slave,ocw1
    nop
    pop eax
    ret



FloppyInitHandler:
    pushad
    mov byte [floppystatus],0xff
    mov al,20h
    out 20h,al
    popad
    iret

setinterrupt:
    push ebp
    mov ebp,esp
    pushad
    mov eax,0x26
    shl eax,3
    mov ebx,FloppyInitHandler
    mov [es:eax],bx
    add eax,2
    mov cx,cs
    mov [es:eax],cx
    add eax,2
    mov cx,0x8e00
    mov [es:eax],cx
    add eax,2
    shr ebx,16
    mov [es:eax],bx
    popad
    leave
    ret
    
section .data
    floppystatus db   0
