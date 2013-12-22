#include <process.h>
#include <chouryos.h>
#include <syscall.h>
#include <floppy.h>
#include <hd.h>
#include <keyboad.h>
#include <schedule.h>

u32 curpid;                         //当前正在运行进程号

int* __errno() {
    return (int *)12000;
}


void Setinterrupt(int into,void f()) {
    INTHER[into]=f;
}

void set8253(u16 time) {
    outp(0x43,0x36);     /* binary, mode 3, LSB/MSB, ch 0 */
    outp(0x40,time & 0xff);    /* LSB */
    outp(0x40,time >> 8 );
}



void init() {
    int i;
    set8253(0xffff);
    Setinterrupt(0x20,TimerInitHandler);
    Setinterrupt(0x21,KeyBoadHandler);
    Setinterrupt(0x26,FloppyIntHandler);
    Setinterrupt(0x2e,HdIntHandler);
    Setinterrupt(80,(void (*)())syscall);
    outp(0x21,inp(0x21)&0xfd);      //开启键盘中断
    outp(0x21,inp(0x21)&0xfe);      //开启时钟中断
    outp(0x21,inp(0x21)&0xfb);      //允许从片中断
    outp(0xa1,inp(0xa1)&0xbf);      //开启硬盘中断
    curpid=0;                       //初始化进程0,即闲逛进程
    PROTABLE[curpid].status=running;
    PROTABLE[curpid].pid=0;
    PROTABLE[curpid].ppid=0;
    PROTABLE[curpid].ldt=(LDT_START+curpid)<<3;
    PROTABLE[curpid].cdt.base0_23=0;
    PROTABLE[curpid].cdt.base24_31=0;
    PROTABLE[curpid].cdt.limit0_15=0xffff;
    PROTABLE[curpid].cdt.limit16_19=0xf;
    PROTABLE[curpid].cdt.S=1;
    PROTABLE[curpid].cdt.D=1;
    PROTABLE[curpid].cdt.L=0;
    PROTABLE[curpid].cdt.P=1;
    PROTABLE[curpid].cdt.G=1;
    PROTABLE[curpid].cdt.DPL=3;
    PROTABLE[curpid].cdt.Type=DA_E | DA_WR;


    PROTABLE[curpid].ddt.base0_23=0;
    PROTABLE[curpid].ddt.base24_31=0;
    PROTABLE[curpid].ddt.limit0_15=0xffff;
    PROTABLE[curpid].ddt.limit16_19=0xf;
    PROTABLE[curpid].ddt.S=1;
    PROTABLE[curpid].ddt.D=1;
    PROTABLE[curpid].ddt.L=0;
    PROTABLE[curpid].ddt.P=1;
    PROTABLE[curpid].ddt.G=1;
    PROTABLE[curpid].ddt.DPL=3;
    PROTABLE[curpid].ddt.Type=DA_WR;
    
    PROTABLE[curpid].ksdt.base0_23=0x400000-KSL;
    PROTABLE[curpid].ksdt.base24_31=0;
    PROTABLE[curpid].ksdt.limit0_15=KSL;
    PROTABLE[curpid].ksdt.limit16_19=0;
    PROTABLE[curpid].ksdt.S=1;
    PROTABLE[curpid].ksdt.D=1;
    PROTABLE[curpid].ksdt.L=0;
    PROTABLE[curpid].ksdt.P=1;
    PROTABLE[curpid].ksdt.G=0;
    PROTABLE[curpid].ksdt.DPL=0;
    PROTABLE[curpid].ksdt.Type=DA_WR;

    PROTABLE[curpid].reg.ss=L_DDT;
    PROTABLE[curpid].reg.oesp=0x400000-KSL;
    PROTABLE[curpid].reg.cs=L_CDT;
    PROTABLE[curpid].reg.eip=(u32)process0;
    PROTABLE[curpid].reg.eflags=0x1202;
    PROTABLE[curpid].reg.ds=L_DDT;
    PROTABLE[curpid].reg.es=L_DDT;
    PROTABLE[curpid].reg.fs=VGA_DT<<3;
    PROTABLE[curpid].reg.gs=L_DDT;

    for(i=1; i<MAX_PROCESS; ++i) {
        PROTABLE[i].status=unuse;
    }

    PROTABLE[curpid].file[0].isused=1;               //for standard input
    PROTABLE[curpid].file[0].type=TTY;
    PROTABLE[curpid].file[1].isused=1;               //for standard output
    PROTABLE[curpid].file[1].type=TTY;
    PROTABLE[curpid].file[2].isused=1;               //for standard errer
    PROTABLE[curpid].file[2].type=TTY;
    for(i=3; i<MAX_FD; i=i+1) {
        PROTABLE[curpid].file[i].isused=0;
    }
    
    PROTABLE[curpid].waitresource=0;
    
    GDT[LDT_START].base0_23=((u32)&PROTABLE[curpid].cdt)&0xffffff;
    GDT[LDT_START].base24_31=(u32)&PROTABLE[curpid].cdt >> 24;
    GDT[LDT_START].limit0_15=24;
    GDT[LDT_START].limit16_19=0;
    GDT[LDT_START].S=0;
    GDT[LDT_START].D=1;
    GDT[LDT_START].L=0;
    GDT[LDT_START].P=1;
    GDT[LDT_START].G=0;
    GDT[LDT_START].DPL=0;
    GDT[LDT_START].Type=DA_LDT;


    TSS.ss0=L_KSDT;
    TSS.esp0=KSL;


    GDT[TSS_DT].base0_23=((u32)&TSS)&0xffffff;
    GDT[TSS_DT].base24_31=(u32)&TSS >> 24;
    GDT[TSS_DT].limit0_15=104;
    GDT[TSS_DT].limit16_19=0;
    GDT[TSS_DT].S=0;
    GDT[TSS_DT].D=1;
    GDT[TSS_DT].L=0;
    GDT[TSS_DT].P=1;
    GDT[TSS_DT].G=0;
    GDT[TSS_DT].DPL=0;
    GDT[TSS_DT].Type=DA_ATSS;

    sti();
    initfs();
    cli();
    movetouse(&(PROTABLE[curpid]));
}

#include <fcntl.h>
#include <unistd.h>
#include <string.h>

void puts(const char *s){
    write(STDOUT_FILENO,s,strlen(s));
}

void process0(void) {
    puts("The process 0 is started!\n");
    if(fork()==0) {
        puts("I'am child process!\n");
        execve("exe.elf",NULL,NULL);
        while(1);
    } else {
        puts("I forked a process!\n");
        while(1);
    }
    while(1);
}

