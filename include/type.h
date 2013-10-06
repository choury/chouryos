#include <sys/types.h>

#ifndef __TYPE_H__
#define __TYPE_H__

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int   uint32;

typedef uint8   u8;
typedef uint16 u16;
typedef uint32 u32;



typedef struct{
    u16 offset0_15;
    u16 tcss;               //target code segment selector
    u8  reserved;
    u8  Type:4;
    u8  S:1;                //must be 0
    u8  DPL:2;
    u8  P:1;
    u16 offset16_31;
}__attribute__ ((packed)) gate;


typedef struct{
    u16 limit0_15;
    u16 base0_15;
    u8 base16_23;
    u8 Type:4;
    u8 S:1;
    u8 DPL:2;
    u8 P:1;
    u8 limit16_19:4;
    u8 AVL:1;
    u8 L:1;
    u8 D_B:1;
    u8 G:1;
    u8 base24_31;

}__attribute__ ((packed)) ss;



#endif
