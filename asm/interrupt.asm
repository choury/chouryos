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
    mov ax, ss
    cmp ax, L_KSDT
    je rein%+i
    push ds
    push es
    push fs
    push gs
    mov ax, KERNELDATA_DT
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov ax, L_KSDT
    mov ss, ax
    mov esp, KSL
    call [INTHER+i*4]
    cli
    mov eax, [prolen]
    mul dword [curpid]
    add eax, PROTABLE
    mov esp, eax
    mov ax, ds
    mov ss, ax
    pop gs
    pop fs
    pop es
    pop ds
    popad
    iret
rein%+i:
    call [INTHER+i*4]
    popad
    iret
%assign i i+1
%endrep


int80:
    pushad
    mov ebp, eax
    mov ax, ss
    cmp ax, L_KSDT
    je rein80
    push ds
    push es
    push fs
    push gs
    mov ax, KERNELDATA_DT
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov ax, L_KSDT
    mov ss, ax
    mov esp, KSL
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
    mov ebx, eax
    mov eax, [prolen]
    mul dword [curpid]
    add eax, PROTABLE
    mov esp, eax
    mov ax, ds
    mov ss, ax
    mov [esp+44], ebx
    pop gs
    pop fs
    pop es
    pop ds
    popad
    iret
rein80:
    popad
    mov eax, -1
    iret



%assign i 81
%rep 174
int%+i:
    pushad
    mov ax, ss
    cmp ax, L_KSDT
    je rein%+i
    push ds
    push es
    push fs
    push gs
    mov ax, KERNELDATA_DT
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov ax, L_KSDT
    mov ss, ax
    mov esp, KSL
    call [INTHER+i*4]
    cli
    mov eax, [prolen]
    mul dword [curpid]
    add eax, PROTABLE
    mov esp, eax
    mov ax, ds
    mov ss, ax
    pop gs
    pop fs
    pop es
    pop ds
    popad
    iret
rein%+i:
    call [INTHER+i*4]
    popad
    iret
%assign i i+1
%endrep

