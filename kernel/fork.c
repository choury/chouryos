#include <syscall.h>
#include <chouryos.h>
#include <string.h>

/*
 fork
 Create a new process
 */

int sys_fork() {
    int i;
    for(i=0;i<MAX_PROCESS;++i){
        if(PROTABLE[i].isused==0)break;
    }
    if(i==MAX_PROCESS){
        errno = EAGAIN;
        return -1;
    }
    memcpy(PROTABLE+i,PROTABLE+curpid,sizeof(process));
    memcpy(GDT+LDT_START+i,GDT+LDT_START+curpid,sizeof(ss));
    PROTABLE[i].reg.eax=0;
    PROTABLE[i].ldt=(LDT_START+curpid+i)<<3;
    PROTABLE[i].pid=i;
    PROTABLE[i].ppid=curpid;

    GDT[LDT_START+i].base0_23=((u32)&PROTABLE[i].cdt)&0xffffff;
    GDT[LDT_START+i].base24_31=(u32)&PROTABLE[i].cdt >> 24;
    return i;
}
