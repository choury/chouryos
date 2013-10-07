;mem map  0---2047      idt
;         2048---4095   gdt
;         4096---4607   floopy buffer
;         4608---4609   console line number
;         4610---4611   console colume number
;         4612---4621   keyboad buffer
;         4622~~~~~~~   keyboad head
;         4623~~~~~~~   keyboad tail
;         4624~~~~~~~   floppy status
;         4625~~~~~~~   reenter kernel
;         5000---5103   tss
;         10240--11264  realinthandler table
;         12000--12003  current pid
;         12004--12007  return statck top
;         0xB8000       console buffer
;         1M------      process table
;         2M------      kernel


IDT_START equ 0
GDT_START equ 2048

INTHER    equ 10240

KERNELCODE_DT equ 8
KERNELDATA_DT equ 16
TSS_DT        equ 24
LDT_START     equ 32

REENTER         equ 4625
STACKTOP        equ 12004


    extern init
    global inp
    global outp
    global outpw
    global movetouse
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
    mov es,ax
    mov fs,ax
    mov gs,ax
    mov esp,0x2ffffe
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
    inc byte [REENTER]
    jnz rein%+i
    push ds
    push es
    push fs
    push gs
    mov ax, KERNELDATA_DT
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov esp, 0x2ffffe
    call [INTHER+i*4]
    mov esp, [STACKTOP]
    pop gs
    pop fs
    pop es
    pop ds
    popad
    dec byte [REENTER]
    iret
rein%+i:
    call [INTHER+i*4]
    popad
    dec byte [REENTER]
    iret
%assign i i+1
%endrep


int80:
    inc byte [REENTER]
    jnz rein80
    pushad
    push ds
    push es
    push fs
    push gs
    sti
    mov ax, KERNELDATA_DT
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, [esp+44]
    mov esp, 0x2ffffe
    push edi
    push esi
    push edx
    push ecx
    push ebx
    push eax
    call [INTHER+80*4]
    mov esp, [STACKTOP]
    mov [esp+44], eax
    pop gs
    pop fs
    pop es
    pop ds
    popad
    dec byte [REENTER]
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
    dec byte [REENTER]
    iret



%assign i 81
%rep 174
int%+i:
    pushad
    inc byte [REENTER]
    jnz rein%+i
    push ds
    push es
    push fs
    push gs
    mov ax, KERNELDATA_DT
    mov ds, ax
    mov esp, 0x2ffffe
    call [INTHER+i*4]
    mov esp, [STACKTOP]
    pop gs
    pop fs
    pop es
    pop ds
    popad
    dec byte [REENTER]
    iret
rein%+i:
    call [INTHER+i*4]
    popad
    dec byte [REENTER]
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
    dec byte [REENTER]
    iret

