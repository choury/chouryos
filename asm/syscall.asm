global write
global read
global open
global close
global fstat
global lseek
global gettimeofday
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

gettimeofday:
    push ebp
    mov ebp, esp
    push ebx
    push ecx
    mov eax, 8
    mov ebx, [ebp+8]
    mov ecx, [ebp+12]
    int 80
    pop ecx
    pop ebx
    leave
    ret

execve:
    push ebp
    mov ebp, esp
    push ebx
    push ecx
    push edx
    mov eax, 9
    mov ebx, [ebp+8]
    mov ecx, [ebp+12]
    mov edx, [ebp+16]
    int 80
    pop edx
    pop ecx
    pop ebx
    leave
    ret


;(x+y*800)*3
setpoint:
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
    shl ecx,8
    add ebx,ecx
    mov ecx,eax
    shl ecx,5
    add ebx,ecx
    mov ecx,ebx
    shl ebx,1
    add ebx, ecx
    mov eax,[ebp+16]
    mov ecx,eax
    shr ecx,16
    mov [fs:ebx],ax
    mov [fs:ebx+2], cl
    pop ecx
    pop ebx
    leave
    ret

fstat:
isatty:
    ret


