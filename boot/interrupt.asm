%include "../asm/asm.h"

    global  Init8259
    global  setinterrupt
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
    call setinterrupt
    pop eax
    ret


setinterrupt:
    push ebp
    mov ebp,esp
    pushad
%assign i 32
%rep 16

    mov eax,i
    shl eax,3
    mov ebx,int%+i
    mov [es:eax],bx
    add eax,2
    mov cx,KCODE_DT
    mov [es:eax],cx
    add eax,2
    mov cx,0xee00
    mov [es:eax],cx
    add eax,2
    shr ebx,16
    mov [es:eax],bx

%assign i i+1
%endrep

    popad
    leave
    ret


%assign i 32
%rep 16
int%+i:
    pushad
    call [INTHER+i*4]
    popad
    iret
%assign i i+1
%endrep
