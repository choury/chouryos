#include <process.h>
#include <chouryos.h>
#include <syscall.h>
#include <floppy.h>
#include <hd.h>
#include <keyboad.h>



int reenter=0;
u32 curpid;
process *stacktop;
int* __errno() {
    return (int *)12000;
}


void setinterrupt(int into,void f()) {
    INTHER[into]=f;
}

void set8253(u16 time) {
    outp(0x43,0x36);     /* binary, mode 3, LSB/MSB, ch 0 */
    outp(0x40,time & 0xff);    /* LSB */
    outp(0x40,time >> 8 );
}


void TimerInitHandler() {
    outp(0x20,0x20);
    if(!reenter) {
        if((curpid==0) && (PROTABLE[1].isused)) {
            curpid=1;
        } else {
            curpid=0;
        }
        TSS.esp0=(u32)&(PROTABLE[curpid].pid);
        stacktop=PROTABLE+curpid;
        lldt(PROTABLE[curpid].ldt);
    }
}


void init() {
    int i;
    set8253(0xffff);
    setinterrupt(0x20,TimerInitHandler);
    setinterrupt(0x21,KeyBoadHandler);
    setinterrupt(0x26,FloppyInitHandler);
    setinterrupt(0x2e,HdInitHandler);
    setinterrupt(80,(void (*)())syscall);
    outp(0x21,inp(0x21)&0xfd);      //开启键盘中断
    outp(0x21,inp(0x21)&0xfe);      //开启时钟中断
    outp(0xa1,inp(0xa1)&0xbf);      //开启硬盘中断
    curpid=0;
    PROTABLE[curpid].isused=1;
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

    PROTABLE[curpid].reg.ss=(1<<3)|7;
    PROTABLE[curpid].reg.oesp=0x3ffffe;
    PROTABLE[curpid].reg.cs=(0<<3)|7;
    PROTABLE[curpid].reg.eip=(u32)process0;
    PROTABLE[curpid].reg.eflags=0x1202;
    PROTABLE[curpid].reg.ds=(1<<3)|7;
    PROTABLE[curpid].reg.es=(1<<3)|7;
    PROTABLE[curpid].reg.fs=VGA_DT<<3;
    PROTABLE[curpid].reg.gs=(1<<3)|7;

    for(i=1; i<MAX_PROCESS; ++i) {
        PROTABLE[i].isused=0;
    }

    PROTABLE[curpid].file[0].isused=1;               //for standard input
    PROTABLE[curpid].file[0].dev=TTY;
    PROTABLE[curpid].file[1].isused=1;               //for standard output
    PROTABLE[curpid].file[1].dev=TTY;
    PROTABLE[curpid].file[2].isused=1;               //for standard errer
    PROTABLE[curpid].file[2].dev=TTY;
    for(i=3; i<MAX_FD; i=i+1) {
        PROTABLE[curpid].file[i].isused=0;
    }

    GDT[LDT_START].base0_23=((u32)&PROTABLE[curpid].cdt)&0xffffff;
    GDT[LDT_START].base24_31=(u32)&PROTABLE[curpid].cdt >> 24;
    GDT[LDT_START].limit0_15=16;
    GDT[LDT_START].limit16_19=0;
    GDT[LDT_START].S=0;
    GDT[LDT_START].D=1;
    GDT[LDT_START].L=0;
    GDT[LDT_START].P=1;
    GDT[LDT_START].G=0;
    GDT[LDT_START].DPL=0;
    GDT[LDT_START].Type=DA_LDT;


    TSS.ss0=KERNELDATA_DT<<3;
    TSS.esp0=(u32)&(PROTABLE[curpid].pid);
    stacktop=PROTABLE+curpid;


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

    movetouse(&(PROTABLE[curpid]));
}


#include <unistd.h>

void process0(void) {
    printf("HD0:\n");
    printf("drivers:%d,heads:%d,cylinders:%d,spt:%d\n",
           HdInfo[0].drivers,HdInfo[0].heads,HdInfo[0].cylindersh<<8 | HdInfo[0].cylindersl,HdInfo[0].spt);
    while(1) {
        char a;
        read(1,&a,1);
        write(1,&a,1);
    }
}

