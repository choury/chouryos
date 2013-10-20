#include <syscall.h>
#include <chouryos.h>
#include <string.h>

static int line=1;
static int colume=0;

/*
 write
 Write a character to a file. `libc' subroutines will use this system routine for output to all files, including stdout
 Returns -1 on error or number of bytes sent
 */

int sys_write(int fd,const void *ptr,size_t len) {
    size_t count=0;
    if((fd < 0) || (fd >= MAX_FD) || (!PROTABLE[curpid].file[fd].isused)) {
        errno=EBADF;
        return -1;
    }
    switch(PROTABLE[curpid].file[fd].dev) {
    case TTY:
        while(count<len) {
            char c=((char *)ptr)[count++];
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
            if(line==25){
                memcpy(charbuff,charbuff+80,80*26*2);
                line=24;
            }
        }
        outp(0x3d4,14);
        outp(0x3d5,(line*80+colume)>>8);
        outp(0x3d4,15);
        outp(0x3d5,line*80+colume);
        return len;
    case NOMAL_FILE:
        return file_write(PROTABLE[curpid].file+fd,ptr,len);
    default:
        return -1;
    }
}

