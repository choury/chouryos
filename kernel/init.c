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


void defultinthandle(int no,int code){
    printf("The int %d happened:%d!\n",no,code);
    while(1);
}


void init() {
    int i;
    set8253(0xffff);
    for(i=0;i<255;i++){
        Setinterrupt(i,defultinthandle);
    }
    Setinterrupt(0x20,TimerInitHandler);
    Setinterrupt(0x21,KeyBoadHandler);
    Setinterrupt(0x26,FloppyIntHandler);
    Setinterrupt(0x2e,HdIntHandler);
    Setinterrupt(0x2f,HdIntHandler);
    Setinterrupt(80,(void (*)())syscall);
    outp(0x21,inp(0x21)&0xfd);      //开启键盘中断
    outp(0x21,inp(0x21)&0xfe);      //开启时钟中断
    outp(0x21,inp(0x21)&0xfb);      //允许从片中断
    outp(0xa1,inp(0xa1)&0xbf);      //开启硬盘中断
    outp(0xa1,inp(0xa1)&0x7f);      //开启第二硬盘中断
    curpid=0;                       //初始化进程0,即闲逛进程
    PROTABLE[curpid].status=running;
    PROTABLE[curpid].pid=0;
    PROTABLE[curpid].ppid=0;
    
    GDT[UCODEI].base0_23=0;
    GDT[UCODEI].base24_31=0;
    GDT[UCODEI].limit0_15=0xffff;
    GDT[UCODEI].limit16_19=0xf;
    GDT[UCODEI].S=1;
    GDT[UCODEI].D=1;
    GDT[UCODEI].L=0;
    GDT[UCODEI].P=1;
    GDT[UCODEI].G=1;
    GDT[UCODEI].DPL=3;
    GDT[UCODEI].Type=DA_E | DA_WR;

    GDT[UDATAI].base0_23=0;
    GDT[UDATAI].base24_31=0;
    GDT[UDATAI].limit0_15=0xffff;
    GDT[UDATAI].limit16_19=0xf;
    GDT[UDATAI].S=1;
    GDT[UDATAI].D=1;
    GDT[UDATAI].L=0;
    GDT[UDATAI].P=1;
    GDT[UDATAI].G=1;
    GDT[UDATAI].DPL=3;
    GDT[UDATAI].Type=DA_WR;

    PROTABLE[curpid].reg.ss=KSTACK_DT;
    PROTABLE[curpid].reg.oesp=0x400000-KSL;
    PROTABLE[curpid].reg.cs=UCODE_DT;
    PROTABLE[curpid].reg.eip=(u32)process0;
    PROTABLE[curpid].reg.eflags=0x1202;
    PROTABLE[curpid].reg.ds=UDATA_DT;
    PROTABLE[curpid].reg.es=UDATA_DT;
    PROTABLE[curpid].reg.fs=VGA_DT;
    PROTABLE[curpid].reg.gs=UDATA_DT;

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


    TSS.ss0=KSTACK_DT;
    TSS.esp0=0x400000;


    GDT[TSSI].base0_23=((u32)&TSS)&0xffffff;
    GDT[TSSI].base24_31=(u32)&TSS >> 24;
    GDT[TSSI].limit0_15=104;
    GDT[TSSI].limit16_19=0;
    GDT[TSSI].S=0;
    GDT[TSSI].D=1;
    GDT[TSSI].L=0;
    GDT[TSSI].P=1;
    GDT[TSSI].G=0;
    GDT[TSSI].DPL=0;
    GDT[TSSI].Type=DA_ATSS;

    sti();
    putstring("I will init fs\n");
    initfs();
    putstring("I inited fs\n");
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
//        execve("exe.elf",NULL,NULL);
        while(1);
    } else {
        puts("I forked a process!\n");
        while(1);
    }
}

