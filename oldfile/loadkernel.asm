%include "pm.h"
use32
    extern  open
    extern  read
    extern  close
    extern  puts
    global  loadkernel
    


    
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
    test eax, eax
    jz __rtcdstret
    mov ecx,eax
    mov esi,filebuff
__rtcdst1:
    mov bl,[esi]
    mov [edi],bl
    inc esi
    inc edi
    loop __rtcdst1
    jmp __rtcdst2
__rtcdstret:
    add esp,20
    popad
    leave
    ret
    
    
loadkernel:
    push ebp
    mov ebp,esp
    pushad
    sub esp,10
    mov dword [esp],kernelname
    call open
    cmp eax,0
    jl  __exece1
    mov [esp],eax
    mov dword [esp+4],0x200000
    call readtocalldst
    call  0x200000
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


section .date
    warning db "There's no kernel can be loader!" 
            db 0
    kernelname db "chouryos" 
            db 0
    filebuff times 1024 db 0
