IDT_START equ 0
GDT_START equ 2048

INTHER    equ 10240

KERNELCODE_DT equ 8
KERNELDATA_DT equ 16

LDT_START equ 4

    extern init
    global inp
    global outp
    global outpw
use32
start:
    cli
    xor eax,eax
    mov ebx,GDT_START+KERNELCODE_DT
    mov [ebx + 2],ax
    shr eax,16
    mov [ebx + 4],al
    mov [ebx + 7],ah
    mov ebx,GDT_START+KERNELDATA_DT
    mov [ebx + 2],ax
    shr eax,16
    mov [ebx + 4],al
    mov [ebx + 7],ah
    mov ax,KERNELDATA_DT
    mov ds,ax
    mov ss,ax
    mov esp,0x1fffe
    jmp KERNELCODE_DT:next
next:
    call setinterrupt
    call init
    jmp $



outp:
    push ebp
    mov ebp,esp
    push edx
    mov edx,[ebp+8]
    mov eax,[ebp+12]
    out dx,al
    nop
    nop
    pop edx
    leave
    ret

outpw:
    push ebp
    mov ebp,esp
    push edx
    mov edx,[ebp+8]
    mov eax,[ebp+12]
    out dx,ax
    nop
    nop
    pop edx
    leave
    ret


inp:
    push ebp
    mov ebp,esp
    push edx
    mov edx,[ebp+0x8]
    mov eax,0
    in  al,dx
    pop edx
    leave
    ret

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
    mov cx,cs
    mov [es:eax],cx
    add eax,2
    mov cx,0x8e00
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
    push eax
    push ecx
    push edx
    call [INTHER+i*4]
    pop edx
    pop ecx
    pop eax
    iret
%assign i i+1
%endrep

int80:
    sti
    push edi
    push esi
    push edx
    push ecx
    push ebx
    push eax
    call [INTHER+80*4]
    add esp, 24
    iret

%assign i 81
%rep 174
int%+i:
    push eax
    push ecx
    push edx
    call [INTHER+i*4]
    pop edx
    pop ecx
    pop eax
%assign i i+1
%endrep


movetouse:
    iret

