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
    register_status *prs=(register_status*)(0x400000-sizeof(register_status));            //父进程系统栈压入了一个register_status的副本
    memcpy(PROTABLE+i,PROTABLE+curpid,sizeof(process));                                   //复制进程控制块
    memcpy(&PROTABLE[i].reg,prs,sizeof(register_status));                                 //复制寄存器值，从父进程系统栈
    memcpy(GDT+LDT_START+i,GDT+LDT_START+curpid,sizeof(ss));                              //复制ldt
    PROTABLE[i].status=ready;
    PROTABLE[i].reg.eax=0;
    PROTABLE[i].ldt=(LDT_START+i)<<3;
    PROTABLE[i].pid=i;
    PROTABLE[i].ppid=curpid;
    PROTABLE[i].reg.oesp=prs->oesp+0x100000;
    PROTABLE[i].reg.ebp=prs->ebp+0x100000;
    memcpy((void *)PROTABLE[i].reg.oesp,(void *)prs->oesp,0x400000-prs->oesp);
    GDT[LDT_START+i].base0_23=((u32)&PROTABLE[i].cdt)&0xffffff;
    GDT[LDT_START+i].base24_31=(u32)&PROTABLE[i].cdt >> 24;
    sti();
    return i;
}
