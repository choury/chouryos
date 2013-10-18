global write
global read
global open
global close
global fstat
global lseek
global isatty
global sbrk
global fork
global setpoint


use32

write:
    push ebp
    mov ebp,esp
    push ebx
    push ecx
    push edx
    mov eax, 1
    mov ebx, [ebp+8]
    mov ecx, [ebp+12]
    mov edx, [ebp+16]
    int 80
    pop edx
    pop ecx
    pop ebx
    leave
    ret

read:
    push ebp
    mov ebp,esp
    push ebx
    push ecx
    push edx
    mov eax, 2
    mov ebx, [ebp+8]
    mov ecx, [ebp+12]
    mov edx, [ebp+16]
    int 80
    pop edx
    pop ecx
    pop ebx
    leave
    ret

open:
    push ebp
    mov ebp,esp
    push ebx
    push ecx
    push edx
    mov eax, 3
    mov ebx, [ebp+8]
    mov ecx, [ebp+12]
    mov edx, [ebp+16]
    int 80
    pop edx
    pop ecx
    pop ebx
    leave
    ret


close:
    push ebp
    mov ebp, esp
    push ebx
    mov eax, 4
    mov ebx, [ebp+8]
    int 80
    pop ebx
    leave
    ret

sbrk:
    push ebp
    mov ebp, esp
    push ebx
    mov eax, 5
    mov ebx, [ebp+8]
    int 80
    pop ebx
    leave
    ret

fork:
    push ebp
    mov ebp, esp
    mov eax, 6
    int 80
    leave
    ret

lseek:
    push ebp
    mov ebp, esp
    push ebx
    push ecx
    push edx
    mov eax, 7
    mov ebx, [ebp+8]
    mov ecx, [ebp+12]
    mov edx, [ebp+16]
    int 80
    pop edx
    pop ecx
    pop ebx
    leave
    ret

fstat:
isatty:
setpoint:
    ret


