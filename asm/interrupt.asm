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
    inc dword [reenter]
    jnz rein%+i
    push ds
    push es
    push fs
    push gs
    mov ax, KERNELDATA_DT
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov esp, 0x1ffffe
    call [INTHER+i*4]
    cli
    mov esp, [stacktop]
    pop gs
    pop fs
    pop es
    pop ds
    popad
    dec dword [reenter]
    iret
rein%+i:
    call [INTHER+i*4]
    popad
    dec dword [reenter]
    iret
%assign i i+1
%endrep


int80:
    inc dword [reenter]
    jnz rein80
    pushad
    push ds
    push es
    push fs
    push gs
    mov ax, KERNELDATA_DT
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov eax, [esp+44]
    mov esp, 0x1ffffe
    sti
    push edi
    push esi
    push edx
    push ecx
    push ebx
    push eax
    call [INTHER+80*4]
    cli
    mov esp, [stacktop]
    mov [esp+44], eax
    pop gs
    pop fs
    pop es
    pop ds
    popad
    dec dword [reenter]
    iret
rein80:
    sti
    push edi
    push esi
    push edx
    push ecx
    push ebx
    push eax
    call [INTHER+80*4]
    add esp, 4
    pop ebx
    pop ecx
    pop edx
    pop esi
    pop edi
    dec dword [reenter]
    iret



%assign i 81
%rep 174
int%+i:
    pushad
    inc dword [reenter]
    jnz rein%+i
    push ds
    push es
    push fs
    push gs
    mov ax, KERNELDATA_DT
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov esp, 0x1ffffe
    call [INTHER+i*4]
    cli
    mov esp, [stacktop]
    pop gs
    pop fs
    pop es
    pop ds
    popad
    dec dword [reenter]
    iret
rein%+i:
    call [INTHER+i*4]
    popad
    dec dword [reenter]
    iret
%assign i i+1
%endrep

