;mem map  0---0x7ff      idt
;         0x800---0xfff   gdt
;         0x1000---0x11ff   floopy buffer
;         0x1200---0x1267   tss
;         0x1300---0x1500  hdd info
;         0x2800--0x2c00   realinthandler table
;         0xa0000-0xfffff  bios rom     0xB8000       console buffer
;         1M------         process table
;         2M------         kernel


IDT_START equ 0
GDT_START equ 2048

INTHER    equ 10240

KERNELCODE_DT equ 8
KERNELDATA_DT equ 16
VGA_DT        equ 24
TSS_DT        equ 32
LDT_START     equ 40

    extern init
    extern reenter
    extern stacktop
    global inp
    global outp
    global outpw
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



outp:
    push ebp
    mov ebp,esp
    push edx
    mov edx,[ebp+8]
    mov eax,[ebp+12]
    out dx,al
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
    inc byte [reenter]
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
    dec byte [reenter]
    iret
rein%+i:
    call [INTHER+i*4]
    popad
    dec byte [reenter]
    iret
%assign i i+1
%endrep


int80:
    inc byte [reenter]
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
    dec byte [reenter]
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
    dec byte [reenter]
    iret



%assign i 81
%rep 174
int%+i:
    pushad
    inc byte [reenter]
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
    dec byte [reenter]
    iret
rein%+i:
    call [INTHER+i*4]
    popad
    dec byte [reenter]
    iret
%assign i i+1
%endrep


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
    dec byte [reenter]
    iret

