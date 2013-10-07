%include "pm.h"
use32
    extern  open
    extern  read
    extern  close
    extern  puts
    global  loadkernel
    
setidt:
    push ebp
    mov ebp,esp
    pushad
    mov esi,LDT
    mov edi,0x1400
    mov ecx,16
__stidt1:
    mov al,[ds:esi]
    mov [es:edi],al
    inc esi
    inc edi
    loop __stidt1
    popad
    leave
    ret
    
    
readtocalldst:
    push ebp
    mov ebp,esp
    pushad
    sub esp,20
    mov edi,[ebp+12]
__rtcdst2:
    mov eax,[ebp+8]
    mov [esp],eax
    mov dword [esp+4],filebuff
    mov dword [esp+8],1024
    call read
    push fs
    mov bx,DATA-LDT
    or  bx,4
    mov fs,bx
    mov ecx,eax
    mov esi,filebuff
__rtcdst1:
    mov bl,[ds:esi]
    mov [fs:edi],bl
    inc esi
    inc edi
    loop __rtcdst1
    pop fs
    cmp eax,1024
    je __rtcdst2
    add esp,20
    popad
    leave
    ret
    
    
loadkernel:
    push ebp
    mov ebp,esp
    pushad
    call setidt
    sub esp,10
    mov dword [esp],kernelname
    call open
    cmp eax,0
    jl  __exece1
    mov ebx,eax
    mov [esp],eax
    mov dword [esp+4],0x200000
    call readtocalldst
    push ds
    mov  ax,DATA-LDT
    or   ax,4
    mov  ds,ax
    call  (CODE-LDT)|4:0x200000
    pop ds
    jmp __execr
__exece1:
    mov eax,warning
    mov [esp],eax
    call puts
__execr:
    add esp,10
    popad
    leave
    ret

section .ldt
LDT:
    CODE:  Descriptor  0x0,            0x2ff, DA_C   +DA_32+DA_G
    DATA:  Descriptor  0x0,            0x2ff, DA_DRWA+DA_32+DA_G
    
section .date
    warning db "There's no kernel can be loader!" 
            db 0
    kernelname db "chouryos" 
            db 0
    filebuff times 1024 db 0
