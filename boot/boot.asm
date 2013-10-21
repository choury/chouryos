use16
org 7c00h
BaseOfStack     equ     0x7c00    ;  Stack base address, inner 
BaseOfLoader    equ     0x2000    ;  Section loading address of LOADER.BIN
OffsetOfLoader  equ     0x0000    ;  Loading offset of LOADER.BIN
RootDirSectors  equ     14        ;  Root directory sector count 
SecNoOfRootDir  equ     19        ;  1st sector of root directory 
SecNoOfFAT1     equ     1         ;  1st sector of FAT1 
DeltaSecNo      equ     17        ;  BPB_(RsvdSecCnt+NumFATs*FATSz) -2 
;  Start sector of file space =
section .text
__s:
;  Floppy header of FAT12 
    jmp     LABEL_START ;  Start to boot. 
    nop                 ;  nop required 
BS_OEMName:         db  "ChouryOs"    ;  OEM String, 8 bytes required 
BPB_BytsPerSec:     dw  512           ;  Bytes per sector 
BPB_SecPerCluster:  db   1             ;  Sector per cluster 
BPB_ResvdSecCnt:    dw  1             ;  Reserved sector count 
BPB_NumFATs:        db   2             ;  Number of FATs 
BPB_RootEntCnt:     dw  224           ;  Root entries count 
BPB_TotSec16:       dw  2880          ;  Total sector number 
BPB_Media:          db   0xf0          ;  Media descriptor 
BPB_FATSz16:        dw  9             ;  FAT size(sectors) 
BPB_SecPerTrk:      dw  18            ;  Sector per track 
BPB_NumHeads:       dw  2             ;  Number of magnetic heads 
BPB_HiddSec:        dd  0             ;  Number of hidden sectors 
BPB_TotSec32:       dd  0             ;  If TotSec16 equal 0, this works 
BS_DrvNum:          db   0             ;  Driver number of interrupt 13 
BS_Reserved1:       db   0             ;  Reserved 
BS_BootSig:         db   0x29          ;  Boot signal 
BS_VolID:           dd  0             ;  Volume ID 
BS_VolLab:          db  "ChouryOs0.1" ;  Volume label, 11 bytes required 
BS_FileSysType:     db  "FAT12   "    ;  File system type, 8 bytes required 

;  Initial registers. 
LABEL_START:
    mov     ax,cs
    mov     ds,ax
    mov     es,ax
    mov     ss,ax
    mov     sp,BaseOfStack

    ;  Clear screen 
    mov     ax,0x0600   ;  %ah=6, %al=0 
    mov     bx,0x0700   ;  Black white 
    mov     cx,0        ;  Top left: (0,0) 
    mov     dx,0x184f   ;  Bottom right: (80,50) 
    int     0x10         ;  BIOS int 10h, ah=6: Initialize screen 
    
    mov     cx,0x0039
    mov     ah,0x1
    int     0x10
    
    ;  Display "Booting**" 
    mov     dh,0
    call    DispStr       ;  Display string(index 0)

    ;  Reset floppy 
    xor     ah,ah
    xor     dl,dl       ;  %dl=0: floppy driver 0 
    int     0x13         ;  BIOS int 13h, ah=0: Reset driver 0 

    ;  Find LOADER.BIN in root directory of driver 0 
    mov  word [wSectorNo],SecNoOfRootDir

;  Read root dir sector to memory 
LABEL_SEARCH_IN_ROOT_DIR_BEGIN:
    cmp word  [wRootDirSizeForLoop],0    ;  If searching in root dir 
    jz      LABEL_NO_LOADERBIN          ;  can find  LOADER.BIN ?   
    dec word  [wRootDirSizeForLoop]
    mov     ax,BaseOfLoader
    mov     es,ax                     ;  %es <- BaseOfLoader
    mov     bx,OffsetOfLoader         ;  %bx <- OffsetOfLoader 
    mov     ax,[wSectorNo]             ;  %ax <- sector number in root 
    mov     cl,1
    call    ReadSector
    mov     si,LoaderFileName         ;  %ds:%si -> LOADER  BIN 
    mov     di,OffsetOfLoader         ;  BaseOfLoader<<4+100
    cld
    mov     dx,0x10

; Search for "LOADER  BIN", FAT12 save file name in 12 bytes, 8 bytes for 
;   file name, 3 bytes for suffix, last 1 bytes for '\20'. If file name is
;   less than 8 bytes, filled with '\20'. So "LOADER.BIN" is saved as:
;   "LOADER  BIN"(4f4c 4441 5245 2020 4942 204e). 

LABEL_SEARCH_FOR_LOADERBIN:
    cmp     dx,0                      ;  Read control 
    jz      LABEL_GOTO_NEXT_SECTOR_IN_ROOT_DIR
    dec     dx
    mov     cx,11

LABEL_CMP_FILENAME:
    cmp     cx,0
    jz      LABEL_FILENAME_FOUND    ;  If 11 chars are all identical? 
    dec     cx
    lodsb                           ;  %ds:(%si) -> %al
    cmp     al,[es:di]
    jz      LABEL_GO_ON
    jmp     LABEL_DIFFERENT         ;  Different 

LABEL_GO_ON:
    inc     di
    jmp     LABEL_CMP_FILENAME      ;  Go on loop 

LABEL_DIFFERENT:
    and     di,0xffe0             ;  Go to head of this entry 
    add     di,0x20
    mov     si,LoaderFileName     ;  Next entry 
    jmp     LABEL_SEARCH_FOR_LOADERBIN

LABEL_GOTO_NEXT_SECTOR_IN_ROOT_DIR:
    add word [wSectorNo],1
    jmp     LABEL_SEARCH_IN_ROOT_DIR_BEGIN

;  Not found LOADER.BIN in root dir. 
LABEL_NO_LOADERBIN:
    mov     dh,2
    call    DispStr         ;  Display string(index 2) 
    jmp     $               ;  Infinite loop 

;  Found. 
LABEL_FILENAME_FOUND:
    mov     ax,RootDirSectors
    and     di,0xffe0             ;  Start of current entry, 32 bytes per entry 
    add     di,0x1a               ;  First sector of this file 
    mov     cx,[es:di]
    push    cx                     ;  Save index of this sector in FAT 
    add     cx,ax
    add     cx,DeltaSecNo         ;  LOADER.BIN's start sector saved in %cl 
    mov     ax,BaseOfLoader
    mov     es,ax                 ;  %es <- BaseOfLoader 
    mov     bx,OffsetOfLoader     ;  %bx <- OffsetOfLoader 
    mov     ax,cx                 ;  %ax <- Sector number 

;  Load LOADER.BIN's sector's to memory. 
LABEL_GOON_LOADING_FILE:
    mov     cl,1
    call    ReadSector
    pop     ax             ;  Got index of this sector in FAT 
    call    GetFATEntry
    cmp     ax,0x0fff
    jz      LABEL_FILE_LOADED
    push    ax             ;  Save index of this sector in FAT 
    mov     dx,RootDirSectors
    add     ax,dx
    add     ax,DeltaSecNo
    add     bx,[BPB_BytsPerSec]
    jmp     LABEL_GOON_LOADING_FILE

LABEL_FILE_LOADED:
    mov     dh,1        
    call    DispStr        ;  Display string(index 1) 


;   Jump to LOADER.BIN's start address in memory.

    jmp     BaseOfLoader:OffsetOfLoader



;   Variable table

wRootDirSizeForLoop:    dw  RootDirSectors
wSectorNo:              dw  0       ;  Sector number to read 
bOdd:                   db   0       ;  odd or even? 


;   String table

LoaderFileName:     db  "LOADER     "        ;  File name 
MessageLength       equ   9
BootMessage:        db    "Boot     "        ;  index 0 
Message1:           db    "Loading.."        ;  index 1 
Message2:           db    "No LOADER"        ;  index 2 


;   Routine: DispStr
;   Action: Display a string, string index stored in %dh

DispStr:
    mov     ax,MessageLength
    mul     dh
    add     ax,BootMessage
    mov     bp,ax               ;  String address 
    mov     ax,ds
    mov     es,ax
    mov     cx,MessageLength    ;  String length 
    mov     ax,0x1301           ;  ah = 0x13, al = 0x01(W) 
    mov     bx,0x07             ;  PageNum 0(bh = 0), bw(bl= 0x07)
    mov     dl,0                ;  Start row and column 
    int     0x10                 ;  BIOS INT 10h, display string 
    ret


;   Routine: ReadSector
;   Action: Read %cl Sectors from %ax sector(floppy) to %es:%bx(memory) 
;     Assume sector number is 'x', then:
;       x/(BPB_SecPerTrk) = y,
;       x%(BPB_SecPerTrk) = z.
;     The remainder 'z' PLUS 1 is the start sector number;
;     The quotient 'y' divide by BPB_NumHeads(RIGHT SHIFT 1 bit)is cylinder
;       number;
;     AND 'y' by 1 can got magnetic header.

ReadSector:
    push    ebp
    mov     ebp,esp
    sub     esp,2        ;  Reserve space for saving %cl 
    mov     [ebp-2],cl
    push    bx            ;  Save bx 
    mov     bl,[BPB_SecPerTrk]    ;  %bl: the divider 
    div     bl            ;  'y' in %al, 'z' in %ah 
    inc     ah            ;  z++, got start sector 
    mov     cl,ah        ;  %cl <- start sector number 
    mov     dh,al        ;  %dh <- 'y' 
    shr     al,1         ;  'y'/BPB_NumHeads 
    mov     ch,al        ;  %ch <- Cylinder number(y>>1) 
    and     dh,1         ;  %dh <- Magnetic header(y&1) 
    pop     bx            ;  Restore %bx 
    ; Now, we got cylinder number in %ch, start sector number in %cl, magnetic
    ;   header in %dh. 
    mov     dl,[BS_DrvNum]
GoOnReading:
    mov     ah,2
    mov     al,[ebp-2]    ;  Read %al sectors 
    int     0x13
    jc      GoOnReading     ;  If CF set 1, mean read error, reread. 
    add     esp,2
    pop     ebp
    ret


;   Routine: GetFATEntry
;   Action: Find %ax sector's index in FAT, save result in %ax 

GetFATEntry:
    push    es
    push    bx
    push    ax
    mov     ax,BaseOfLoader
    sub     ax,0x0100
    mov     es,ax           ;  Left 4K bytes for FAT 
    pop     ax
    mov     bx,3
    mul     bx               ;  %dx:%ax = %ax*3 
    mov     bx,2
    div     bx               ;  %dx:%ax/2 
    mov byte [bOdd],dl       ;  store remainder %dx in label bOdd. 

LABEL_EVEN:
    xor     dx,dx           ;  Now %ax is the offset of FATEntry in FAT 
    mov     bx,[BPB_BytsPerSec]
    div     bx               ;  %dx:%ax/BPB_BytsPerSec 
    push    dx
    mov     bx,0
    add     ax,SecNoOfFAT1  ;  %ax <- FATEntry's sector 
    mov     cl,2            ;  Read 2 sectors in 1 time, because FATEntry 
    call    ReadSector        ;  may be in 2 sectors. 
    pop     dx
    add     bx,dx
    mov     ax,[es:bx]     ;  read FAT entry by word(2 bytes) 
    cmp byte [bOdd],0         ;  remainder %dx(see above) == 0 ?
    jz      LABEL_EVEN_2      ;  NOTE: %ah: high address byte, %al: low byte 
    shr     ax,4

LABEL_EVEN_2:
    and     ax,0x0fff

LABEL_GET_FAT_ENTRY_OK:
    pop     bx
    pop     es
    ret

TIMES 510-($-$$) DB 0        ;  Skip to address 0x510. 
dw 0xaa55   ;  Write boot flag to 1st sector(512 bytes) end 

