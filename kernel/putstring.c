#include <string.h>
#include <common.h>


extern int line;
extern int colume;

int putstring(const char* s) {
    while(*s) {
        char c=*(s++);
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
    return 0;
}
