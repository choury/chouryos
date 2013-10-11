#include <stdio.h>
#include <process.h>
#include <chouryos.h>
#include <syscall.h>
#include <floppy.h>
#include <unistd.h>
#include <fcntl.h>
#include <keyboad.h>

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
    int i;
    charbuff[0]=(reenter+'0') | 0x0700;
    if(reenter==0) {
        if(PROTABLE[1].isused){
            CURPID=1;
        }
    }
    TSS.esp0=(u32)&(PROTABLE[CURPID].pid);
    STACKTOP=PROTABLE+CURPID;
    LDT=PROTABLE[CURPID].ldt;
}


void process1() {
    int i=fork();
    write(1,"create a new process",20);
    while(1) {
        charbuff[i+1]++;
    }
}

void init() {
    int i;
    set8253(0xffff);
    setinterrupt(0x20,TimerInitHandler);
    setinterrupt(0x21,KeyBoadHandler);
    setinterrupt(0x26,FloppyInitHandler);
    setinterrupt(80,(void (*)())syscall);
    outp(0x21,inp(0x21)&0xfd);
    outp(0x21,inp(0x21)&0xfe);
    keytail=keyhead=0;
    line=1;
    colume=0;
    floppystatus=0;
    reenter=0;
    CURPID=0;
    PROTABLE[CURPID].isused=1;
    PROTABLE[CURPID].pid=0;
    PROTABLE[CURPID].ppid=0;
    PROTABLE[CURPID].ldt=(LDT_START+CURPID)<<3;
    PROTABLE[CURPID].cdt.base0_23=0;
    PROTABLE[CURPID].cdt.base24_31=0;
    PROTABLE[CURPID].cdt.limit0_15=0xffff;
    PROTABLE[CURPID].cdt.limit16_19=0xf;
    PROTABLE[CURPID].cdt.S=1;
    PROTABLE[CURPID].cdt.D=1;
    PROTABLE[CURPID].cdt.L=0;
    PROTABLE[CURPID].cdt.P=1;
    PROTABLE[CURPID].cdt.G=1;
    PROTABLE[CURPID].cdt.DPL=3;
    PROTABLE[CURPID].cdt.Type=DA_E | DA_WR;


    PROTABLE[CURPID].ddt.base0_23=0;
    PROTABLE[CURPID].ddt.base24_31=0;
    PROTABLE[CURPID].ddt.limit0_15=0xffff;
    PROTABLE[CURPID].ddt.limit16_19=0xf;
    PROTABLE[CURPID].ddt.S=1;
    PROTABLE[CURPID].ddt.D=1;
    PROTABLE[CURPID].ddt.L=0;
    PROTABLE[CURPID].ddt.P=1;
    PROTABLE[CURPID].ddt.G=1;
    PROTABLE[CURPID].ddt.DPL=3;
    PROTABLE[CURPID].ddt.Type=DA_WR;

    PROTABLE[CURPID].reg.ss=(1<<3)|7;
    PROTABLE[CURPID].reg.oesp=0x1ffffe;
    PROTABLE[CURPID].reg.cs=(0<<3)|7;
    PROTABLE[CURPID].reg.eip=(u32)process1;
    PROTABLE[CURPID].reg.eflags=0x1202;
    PROTABLE[CURPID].reg.ds=(1<<3)|7;
    PROTABLE[CURPID].reg.es=(1<<3)|7;
    PROTABLE[CURPID].reg.fs=(1<<3)|7;
    PROTABLE[CURPID].reg.gs=(1<<3)|7;

    for(i=1; i<MAX_PROCESS; ++i) {
        PROTABLE[i].isused=0;
    }

    PROTABLE[CURPID].file[0].isused=1;               //for standard input
    PROTABLE[CURPID].file[1].isused=1;               //for standard output
    PROTABLE[CURPID].file[2].isused=1;               //for standard errer
    for(i=3; i<MAX_FD; i=i+1) {
        PROTABLE[CURPID].file[i].isused=0;
    }

    GDT[LDT_START].base0_23=((u32)&PROTABLE[CURPID].cdt)&0xffffff;
    GDT[LDT_START].base24_31=(u32)&PROTABLE[CURPID].cdt >> 24;
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
    TSS.esp0=(u32)&(PROTABLE[CURPID].pid);


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
    puts("Init secceed!");
    printf("hello newlib\n");

    movetouse(&(PROTABLE[CURPID]));

}
