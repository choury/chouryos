#include <stdio.h>
#include <process.h>
#include <chouryos.h>
#include <syscall.h>
#include <floppy.h>
#include <unistd.h>
#include <fcntl.h>
#include <keyboad.h>

void setinterrupt(int into,void f()){
    INTHER[into]=f;
}

void set8253(u16 time){
    outp(0x43,0x36);     /* binary, mode 3, LSB/MSB, ch 0 */
    outp(0x40,time & 0xff);    /* LSB */
    outp(0x40,time >> 8 );
}

void TimerInitHandler(){
    outp(0x20,0x20);
    charbuff[0]++;
}

void init(){
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
    CURPID=0;
    PROTABLE[CURPID].isused=1;
    PROTABLE[CURPID].pid=0;
    PROTABLE[CURPID].ppid=0;
    PROTABLE[CURPID].ldt=0;
    for(i=1;i<MAX_PROCESS;++i){
        PROTABLE[i].isused=0;
    }
    sti();
    initfs();
    puts("Init secceed!");
    printf("hello newlib\n");
    while(1){
        char a;
        read(0,&a,1);
        write(1,&a,1);
    }
}
