#include <sys/types.h>

#ifndef __TYPE_H__
#define __TYPE_H__


#define uint8  unsigned char
#define uint16 unsigned short
#define uint32 unsigned int

#define u8      uint8
#define u16     uint16
#define u32     uint32


typedef struct{
    u16 offset15_0;
    u16 dts;
    u16 attr;
    u16 offset31_16;
}__attribute__ ((packed)) idt;


typedef struct{
    u8 base31_24;
    u8 flag2_limit19_16;
    u8 flag2_type;
    u8 base23_16;
    u16 base15_0;
    u16 limit15_0;
}__attribute__ ((packed)) dt;



#endif
