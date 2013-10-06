; GDT Descriptor Attributes
;     DA_  : Descriptor Attribute
;     D    : Data Segment
;     C    : Code Segment
;     S    : System Segment
;     R    : Read-only
;     RW   : Read/Write
;     A    : Access 
%assign    DA_32    0x4000  ;32-bit segment 
%assign    DA_G     0x8000  ;G is set

;Descriptor privilege level 
%assign    DA_DPL0  0x00  ;DPL = 0 
%assign    DA_DPL1  0x20  ;DPL = 1 
%assign    DA_DPL2  0x40  ;DPL = 2 
%assign    DA_DPL3  0x60  ;DPL = 3 

;GDT Code- and Data-Segment Types 
%assign    DA_DR    0x90   ;Read-Only 
%assign    DA_DRW   0x92   ;Read/Write 
%assign    DA_DRWA  0x93   ;Read/Write, accessed 
%assign    DA_C     0x98   ;Execute-Only 
%assign    DA_CR    0x9A   ;Execute/Read 
%assign    DA_CCO   0x9C   ;Execute-Only, conforming 
%assign    DA_CCOR  0x9E   ;Execute/Read-Only, conforming 

;GDT System-Segment and Gate-Descriptor Types 
%assign    DA_LDT       0x82    ;LDT 
%assign    DA_TaskGate  0x85    ;Task Gate 
%assign    DA_386TSS    0x89    ;32-bit TSS(Available) 
%assign    DA_386CGate  0x8C    ;32-bit Call Gate 
%assign    DA_386IGate  0x8E    ;32-bit Interrupt Gate 
%assign    DA_386TGate  0x8F    ;32-bit Trap Gate 

;Selector Attributes 
%assign    SA_RPL0  0
%assign    SA_RPL1  1
%assign    SA_RPL2  2
%assign    SA_RPL3  3
%assign    SA_TIG   0
%assign    SA_TIL   4

;MACROS 




; Segment Descriptor data structure.
;   Usage: Descriptor Base, Limit, Attr
;    Base:  4byte 
;    Limit: 4byte (low 20 bits available)
;    Attr:  2byte (lower 4 bits of higher byte are always 0)

%macro Descriptor 3
    dw    %2 & 0FFFFh                
    dw    %1 & 0FFFFh               
    db    (%1 >> 16) & 0FFh           
    dw    ((%2 >> 8) & 0F00h) | (%3 & 0F0FFh)    
    db    (%1 >> 24) & 0FFh            
%endmacro



; Gate Descriptor data structure.
;   Usage: Gate Selector, Offset, PCount, Attr
;    Selector:  2byte
;    Offset:    4byte
;    PCount:    byte
;    Attr:      byte 
%macro Gate  4
    dw     (%2 & 0xFFFF)
    dw     %1
    dw     (%3 & 0x1F) | ((%4 << 8) & 0xFF00)
    dw     ((%2 >> 16) & 0xFFFF)
%endmacro