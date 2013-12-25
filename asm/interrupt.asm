%include "asm.h"
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
    push ebx
%assign i 0
%rep 255

    mov eax,i
    shl eax,3
    mov ebx,int%+i
    mov [eax],bx
    add eax,2
    mov cx,KERNELCODE_DT
    mov [eax],cx
    add eax,2
    mov cx,0xee00
    mov [eax],cx
    add eax,2
    shr ebx,16
    mov [eax],bx

%assign i i+1
%endrep


    pop ebx
    leave
    ret


%assign i 0
%rep 80
int%+i:
    pushad
    push ds
    push es
    push fs
    push gs
    mov ax, KERNELDATA_DT
    mov ds, ax
    mov es, ax
    mov gs, ax
    push dword i
    call [INTHER+i*4]
    add esp, 4
    pop gs
    pop fs
    pop es
    pop ds
    popad
    iret
%assign i i+1
%endrep


int80:
    pushad
    push ds
    push es
    push fs
    push gs
    mov ebp, eax
    mov ax, KERNELDATA_DT
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov eax, ebp
    sti
    push edi
    push esi
    push edx
    push ecx
    push ebx
    push eax
    call [INTHER+80*4]
    add esp, 24
    mov [esp+44], eax
    pop gs
    pop fs
    pop es
    pop ds
    popad
    iret



%assign i 81
%rep 174
int%+i:
    pushad
    push ds
    push es
    push fs
    push gs
    mov ax, KERNELDATA_DT
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov ss, ax
    call [INTHER+i*4]
    pop gs
    pop fs
    pop es
    pop ds
    popad
    iret
%assign i i+1
%endrep

