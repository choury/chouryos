    use32
    global  putchar
    global  outp
    global  outpw
    global  inp
    global  SetPoint
    global  copyfloppydata

line    equ  4608
colum   equ  4610

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

    
SetPoint:
    push ebp
    mov ebp,esp
    push ebx
    push ecx
    mov ebx,[ebp+8]
    mov eax,[ebp+12]
    mov ecx,eax
    shl ecx,9
    add ebx,ecx
    mov ecx,eax
    shl ecx,7
    add ebx,ecx
    mov ecx,ebx
    shl ebx,1
    mov eax,[ebp+16]
    mov ecx,eax
    shr ecx,16
    mov [fs:ebx],ax
    pop ecx
    pop ebx
    leave
    ret
    
putchar:
    push ebp
    mov ebp,esp
    pushad
    mov ax,[es:line]
    mov bl,160
    mul bl
    movzx ebx,ax
    mov si,[es:colum]
    movzx esi,si
    mov eax,[ebp+0x08]
    cmp al,0ah
    je __putc2
    cmp al,0dh
    je __putc2
    cmp al,08h
    je __putc3
    mov ah,07h
    mov [gs:ebx+esi*2],ax
    inc si
    cmp si,80
    jne __putc1
__putc2:
    mov si,0
    inc word [es:line]
    cmp word [es:line],25
    je  __putc4
__putc1:
    mov word [es:colum],si
    mov ax,word [es:line]
    mov bl,80
    mul bl
    mov bx,ax
    add bx,si
    mov al,14
    mov dx,0x3d4
    out dx,al
    mov al,bh
    mov dx,0x3d5
    out dx,al
    
    mov al,15
    mov dx,0x3d4
    out dx,al
    mov al,bl
    mov dx,0x3d5
    out dx,al

    popad
    leave
    ret
__putc3:
    and si,si
    jz __putc1
    dec si
    mov word [gs:ebx+esi*2],0x0720
    jmp __putc1
__putc4:
    mov ax,0601h    ; %ah=6, %al=0 
    mov bx,0700h    ; Black white 
    mov cx,0100h    ; Top left: (0,0) 
    mov dx,184fh    ; Bottom right: (80,50) 
;    int 10h         ; BIOS int 10h, ah=6: Initialize screen
    mov word [es:line],1
    jmp __putc1

copyfloppydata:
    push ebp
    mov ebp,esp
    pushad
    mov edi,[ebp+8]
    mov esi,4096
    mov ecx,512
__c1:
    mov al,[es:esi]
;    mov [esp],al
;    call puthex
    mov [ds:edi],al
    inc esi
    inc edi
    loop __c1
    popad
    leave
    ret

