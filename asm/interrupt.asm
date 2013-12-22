%include "asm.h"

    global  setinterrupt
use32
section .text


setinterrupt:
    push ebp
    mov ebp,esp
    pushad
%assign i 0
%rep 255

    mov eax,i
    shl eax,3
    mov ebx,int%+i
    mov [es:eax],bx
    add eax,2
    mov cx,KERNELCODE_DT
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


%assign i 0
%rep 80
int%+i:
    pushad
    push ds
    mov ax, KERNELDATA_DT
    mov ds, ax
    inc dword [reenter]
    jnz rein%+i
    push es
    push fs
    push gs
    mov es, ax
    mov gs, ax
    mov ax, L_KSDT
    mov ss, ax
    mov esp, 0x1000
    call [INTHER+i*4]
    cli
    dec dword [reenter]
    mov esp, [stacktop]
    pop gs
    pop fs
    pop es
    pop ds
    popad
    iret
rein%+i:
    pop ds
    call [INTHER+i*4]
    dec dword [reenter]
    popad
    iret
%assign i i+1
%endrep


int80:
    pushad
    mov ebp, eax
    push ds
    mov ax, KERNELDATA_DT
    mov ds, ax
    inc dword [reenter]
    jnz rein80
    push es
    push fs
    push gs
    mov es, ax
    mov gs, ax
    mov ax, L_KSDT
    mov ss, ax
    mov esp, 0x1000
    mov eax, ebp
    sti
    push edi
    push esi
    push edx
    push ecx
    push ebx
    push eax
    call [INTHER+80*4]
    cli
    dec dword [reenter]
    mov esp, [stacktop]
    mov [esp+44], eax
    pop gs
    pop fs
    pop es
    pop ds
    popad
    iret
rein80:
    pop ds
    popad
    mov eax, -1
    dec dword [reenter]
    iret



%assign i 81
%rep 174
int%+i:
    pushad
    push ds
    mov ax, KERNELDATA_DT
    mov ds, ax
    inc dword [reenter]
    jnz rein%+i
    push es
    push fs
    push gs
    mov es, ax
    mov gs, ax
    mov ax, L_KSDT
    mov ss, ax
    mov esp, 0x1000
    call [INTHER+i*4]
    cli
    dec dword [reenter]
    mov esp, [stacktop]
    pop gs
    pop fs
    pop es
    pop ds
    popad
    iret
rein%+i:
    pop ds
    call [INTHER+i*4]
    dec dword [reenter]
    popad
    iret
%assign i i+1
%endrep

