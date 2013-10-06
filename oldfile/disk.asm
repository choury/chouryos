    global readsector
    global writesector
use16
section .text
readsector:
    push ebp
    mov ebp,esp
    push dx
    push cx
    push bx
    push es
    mov eax,[ebp+8]
    mov cl,18
    div cl
    mov dh,al
    and dh,1
    shr al,1
    mov ch,al
    inc ah
    mov cl,ah
    mov dl,0
    mov ax,ds
    mov es,ax
    mov ebx,[ebp+12]
__reread:
    mov al,1
    mov ah,2
    int 13h
    jc __reread
    pop es
    pop bx
    pop cx
    pop dx
    o32 leave
    o32 ret

writesector:
    push ebp
    mov ebp,esp
    push dx
    push cx
    push bx
    push es
    mov eax,[ebp+8]
    mov cl,18
    div cl
    mov dh,al
    and dh,1
    shr al,1
    mov ch,al
    inc ah
    mov cl,ah
    mov dl,0
    mov ax,ds
    mov es,ax
    mov ebx,[ebp+12]
__rewrite:
    mov al,1
    mov ah,3
    int 13h
    jc __reread
    pop es
    pop bx
    pop cx
    pop dx
    o32 leave
    o32 ret