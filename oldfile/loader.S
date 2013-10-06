    org 100h
section .data
    table   db "Welcome to choury's OS"
    len     dw  $-table

section .text
start:
    mov ax,0b800h
    mov gs,ax
    mov ax,cs
    mov ds,ax
    
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
    jmp $
