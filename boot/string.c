#include "boot.h"

#define charbuff ((u16 *)0xB8000)

static int line=0;
static int colume=0;

static void* memcpy(void *dest,const void *src,u32 n) {
    u32 i;
    for(i=0; i!=n; ++i) {
        ((char *)dest)[i]=((char *)src)[i];
    }
    return dest;
}




int putstring(const char* s) {
    int count=0;
    while(*s) {
        char c=s[count++];
        if(c=='\n') {
            line++;
            colume=0;
        } else if(c=='\b') {
            colume--;
            if(colume<0) {
                colume=0;
            }
            charbuff[line*80+colume]= 0x0720;
        } else {
            charbuff[line*80+colume++]=c | 0x0700;
        }
        if(colume==80) {
            line++;
            colume=0;
        }
        if(line==25) {
            memcpy(charbuff,charbuff+80,80*26*2);
            line=24;
        }
    }
    outp(0x3d4,14);
    outp(0x3d5,(line*80+colume)>>8);
    outp(0x3d4,15);
    outp(0x3d5,line*80+colume);
    return count;
}
