%include "pm.h"


;mem map  0---2047      idt  
;         2048---4095   gdt
;         4096---4607   floopy buffer
;         4608---4609   console line number
;         4610---4611   console colume number
;         4612---4621   keyboad buffer
;         4622~~~~~~~   keyboad head
;         4623~~~~~~~   keyboad tail
;         4624~~~~~~~   floppy status
;         4624---10239  reserved
;         10240--11264  realinthandler table
;         12000--12003  current pid
;         1M------   loader (it will be process table after found kernel)
;         2M------   kernel


use16
    extern  Init8259
    extern  setinterrupt
    extern  initfs
    extern  loadkernel
    extern  main

section .text
start:
    mov ax,0b800h
    mov gs,ax
    mov ax,cs
    mov ds,ax
    mov es,ax
    mov ss,ax
    mov esp,0xffffe

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
   
    mov ax,4f02h
    mov bx,4111h
;    int 10h
    
    
    mov ax,4f01h
    mov cx,4111h
    mov di,ModeInfoBlock
    int 10h

    mov eax,[PhysBasePtr]
    mov ebx,LABEL_DESC_VGA
    call setdatedt



    cli
    
    xor eax,eax
    mov es,ax
    mov esi,LABEL_GDT
    mov edi,2048
    mov ecx,2048
__movgdt:
    mov al,[ds:esi]
    mov [es:edi],al
    inc si
    inc di
    loop __movgdt
    
    lgdt    [GdtPtr]
    
    lidt    [IdtPtr]
    
    
    ;Open A20 line.
    in  al,0x92
    or  al,0b00000010
    out 0x92,al
    
    ;Enable protect mode, PE bit of CR0.
    mov eax,cr0
    or  eax,1
    mov cr0,eax
    mov ax,LABEL_DESC_VGA-LABEL_GDT
    mov fs,ax
    mov ax,LABEL_DESC_CHAR-LABEL_GDT
    mov gs,ax
    mov ax,LABEL_DESC_ORG-LABEL_GDT
    mov es,ax
    mov ax,LABEL_DESC_DATA-LABEL_GDT
    mov ds,ax
    mov ss,ax
    jmp LABEL_DESC_CODE32-LABEL_GDT:dword LABEL_SEG_CODE32

setdatedt:
    mov     [ebx + 2],ax
    shr     eax,16
    mov     [ebx + 4],al
    mov     [ebx + 7],ah
    ret
    
section .text    
use32
LABEL_SEG_CODE32:
    mov ax,LABEL_DESC_LDT0-LABEL_GDT
    lldt ax
    call Init8259
    call setinterrupt
    sti
    call initfs
    call loadkernel
;Stop here, infinite loop.
    jmp $

    
section .data
    table   db "Welcome to choury's OS"
    len     dw  $-table

    
    
section .vdt
LABEL_GDT:          Descriptor  0,                  0,       0
LABEL_DESC_CODE32:  Descriptor  0x10000,            0xfffff, DA_C + DA_32 + DA_G
LABEL_DESC_DATA:    Descriptor  0x10000,            0xfffff, DA_DRWA+DA_32+ DA_G
LABEL_DESC_ORG:     Descriptor  0,                  0xff,    DA_DRWA+DA_32+ DA_G
LABEL_DESC_VGA:     Descriptor  0,                  0xea,    DA_DRW +DA_G
LABEL_DESC_CHAR:    Descriptor  0xB8000,            0x0,     DA_DRW +DA_G
LABEL_DESC_LDT0:    Descriptor  0x1400,             0x1000 , DA_LDT   


;LABEL_LDT:          Descriptor 
GdtLen equ  $-LABEL_GDT
GdtPtr: dw  2048               ;GDT Limit 
        dd  2048               ;GDT Base


IdtPtr: dw  2048
        dd  0



section .ModeInfoBlock
ModeInfoBlock: 
; Mandatory information for all VBE revisions
ModeAttributes      dw 0 ; mode attributes
WinAAttributes      db 0 ; window A attributes
WinBAttributes      db 0 ; window B attributes
WinGranularity      dw 0 ; window granularity
WinSize             dw 0 ; window size
WinASegment         dw 0 ; window A start segment
WinBSegment         dw 0 ; window B start segment
WinFuncPtr          dd 0 ; real mode pointer to window function
BytesPerScanLine    dw 0 ; bytes per scan line
; Mandatory information for VBE 1.2 and above 
XResolution         dw 0 ; horizontal resolution in pixels or characters
YResolution         dw 0 ; vertical resolution in pixels or characters
XCharSize           db 0 ; character cell width in pixels
YCharSize           db 0 ; character cell height in pixels
NumberOfPlanes      db 0 ; number of memory planes
BitsPerPixel        db 0 ; bits per pixel
NumberOfBanks       db 0 ; number of banks
MemoryModel         db 0 ; memory model type
BankSize            db 0 ; bank size in KB
NumberOfImagePages  db 0 ; number of images
Reserved            db 0 ; reserved for page function
; Direct Color fields (required for direct/6 and YUV/7 memory models)
RedMaskSize         db 0 ; size of direct color red mask in bits
RedFieldPosition    db 0 ; bit position of lsb of red mask
GreenMaskSize       db 0 ; size of direct color green mask in bits
GreenFieldPosition  db 0 ; bit position of lsb of green mask
BlueMaskSize        db 0 ; size of direct color blue mask in bits
BlueFieldPosition   db 0 ; bit position of lsb of blue mask
RsvdMaskSize        db 0 ; size of direct color reserved mask in bits
RsvdFieldPosition   db 0 ; bit position of lsb of reserved mask
DirectColorModeInfo db 0 ; direct color mode attributes
; Mandatory information for VBE 2.0 and above
PhysBasePtr         dd 0 ; physical address for flat memory frame buffer
Reserved0           dd 0 ; Reserved - always set to 0
Reserved1           dw 0 ; Reserved - always set to 0
; Mandatory information for VBE 3.0 and above
LinBytesPerScanLine     dw 0 ; bytes per scan line for linear modes
BnkNumberOfImagePages   db 0 ; number of images for banked modes
LinNumberOfImagePages   db 0 ; number of images for linear modes
LinRedMaskSize          db 0 ; size of direct color red mask (linear modes)
LinRedFieldPosition     db 0 ; bit position of lsb of red mask (linear modes)
LinGreenMaskSize        db 0 ; size of direct color green mask (linear modes)
LinGreenFieldPositiondb db 0 ; bit position of lsb of green mask (linear modes)
LinBlueMaskSize         db 0 ; size of direct color blue mask (linear modes)
LinBlueFieldPosition    db 0 ; bit position of lsb of blue mask (linear modes)
LinRsvdMaskSize         db 0 ; size of direct color reserved mask (linear modes)
LinRsvdFieldPosition    db 0 ; bit position of lsb of reserved mask (linear modes)
MaxPixelClock           dd 0 ; maximum pixel clock (in Hz) for graphics mode
Reserved2 times 190     db 0 ; remainder of ModeInfoBlock         
        
