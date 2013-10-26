use32
    global inp
    global inpw
    global inpd
    global inpn
    global inpwn
    global inpdn
    global outp
    global outpw
    global outpd


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

outpd:
    push ebp
    mov ebp,esp
    push edx
    mov edx,[ebp+8]
    mov eax,[ebp+12]
    out dx,eax
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

inpw:
    push ebp
    mov ebp,esp
    push edx
    mov edx,[ebp+0x8]
    mov eax,0
    in  ax,dx
    pop edx
    leave
    ret

inpd:
    push ebp
    mov ebp,esp
    push edx
    mov edx,[ebp+0x8]
    in  eax,dx
    pop edx
    leave
    ret

inpn:
    push ebp
    mov ebp,esp
    push edx
    push edi
    push ecx
    mov edx,[ebp+0x8]
    mov edi, [ebp+12]
    mov ecx, [ebp+16]
    cld
    rep insb
    pop ecx
    pop edi
    pop edx
    leave
    ret

inpwn:
    push ebp
    mov ebp,esp
    push edx
    push edi
    push ecx
    mov edx,[ebp+0x8]
    mov edi, [ebp+12]
    mov ecx, [ebp+16]
    cld
    rep insw
    pop ecx
    pop edi
    pop edx
    leave
    ret

inpdn:
    push ebp
    mov ebp,esp
    push edx
    push edi
    push ecx
    mov edx,[ebp+0x8]
    mov edi, [ebp+12]
    mov ecx, [ebp+16]
    cld
    rep insd
    pop ecx
    pop edi
    pop edx
    leave
    ret
