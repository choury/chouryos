%include "pm.h"
    global  reboot
    global  shutdown
    global  exec
    extern  main
    extern  open
    extern  read
    use16
;    org 100h

section .text
start:
    mov ax,0b800h
    mov gs,ax
    mov ax,cs
    mov ds,ax
    mov es,ax
    mov ss,ax
    mov esp,0xfffe

    ; Clear screen
    mov ax,0600h    ; %ah=6, %al=0 */
    mov bx,0700h    ; Black white */
    mov cx,0        ; Top left: (0,0) */
    mov dx,184fh    ; Bottom right: (80,50) */
    int 10h         ; BIOS int 10h, ah=6: Initialize screen */


    mov cx,[len]
    mov si,0
    mov bx,table
s1:
    mov ah,0fh
    mov al,[bx]
    mov [gs:si],ax
    inc bx
    add si,2
    loop s1


    mov bh,0
    mov dh,1
    mov dl,0
    mov ah,2
    int 10h
    call main
    jmp $


exec:
    push ebp
    mov ebp,esp
    push edx
    push ecx
    push ebx
    push ds
    sub esp,0x20
    mov eax,[ebp+8]
    mov [esp],eax
    use32
    o16 call open
    use16
    cmp eax,0
    jl  __exec_fail
    mov dword [esp+8],0xfff
    mov dword [esp+4],0x8000
    mov dword [esp],eax
    use32
    o16 call read
    use16
    cmp eax,0
    jl __exec_fail
    call 0x1800:0x0
__exec_fail:
    add esp,0x20
    mov eax,-1
    pop ebx
    pop ecx
    pop edx
    o32 leave
    o32 ret


reboot:
    int 19h

shutdown:
    mov ax, 5301h        ;function 5301h: apm connect real-mode interface
    xor bx, bx           ;device id: 0000h (=system bios)
    int 15h              ;call interrupt: 15h
    mov ax, 530eh        ;function 530eh: apm driver version
    mov cx, 0102h        ;driver version: apm v1.2
    int 15h              ;call interrupt: 15h
    mov ax, 5307h        ;function 5307h: apm set system power state
    mov bl, 01h          ;device id: 0001h (=all devices)
    mov cx, 0003h        ;power state: 0003h (=off)
    int 15h

section .data
    table   db "Welcome to choury's OS"
    len     dw  $-table
    line    dw  1
    colum   dw  0