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
        if(PROTABLE[i].status==unuse)break;
    }
    if(i==MAX_PROCESS){
        errno = EAGAIN;
        return -1;
    }
    cli();
    memcpy(PROTABLE+i,PROTABLE+curpid,sizeof(process));
    memcpy(GDT+LDT_START+i,GDT+LDT_START+curpid,sizeof(ss));
    PROTABLE[i].status=ready;
    PROTABLE[i].reg.eax=0;
    PROTABLE[i].ldt=(LDT_START+i)<<3;
    PROTABLE[i].pid=i;
    PROTABLE[i].ppid=curpid;
    PROTABLE[i].reg.oesp=PROTABLE[curpid].reg.oesp+0x100000;
    PROTABLE[i].reg.ebp=PROTABLE[curpid].reg.ebp+0x100000;
    memcpy((void *)PROTABLE[i].reg.oesp,(void *)PROTABLE[curpid].reg.oesp,0x400000-PROTABLE[curpid].reg.oesp);
    GDT[LDT_START+i].base0_23=((u32)&PROTABLE[i].cdt)&0xffffff;
    GDT[LDT_START+i].base24_31=(u32)&PROTABLE[i].cdt >> 24;
    sti();
    return i;
}
