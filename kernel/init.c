#include <process.h>
#include <syscall.h>
#include <floppy.h>
#include <hd.h>
#include <keyboad.h>
#include <schedule.h>
#include <memory.h>
#include <string.h>

pid_t curpid=0;                         //当前正在运行进程号

int *__errno()
{
    return &(PENV->errno);
}


void Setinterrupt(int into, void f())
{
    INTHER[into] = f;
}

void set8253(uint16 time)
{
    outp(0x43, 0x36);    /* binary, mode 3, LSB/MSB, ch 0 */
    outp(0x40, time & 0xff);   /* LSB */
    outp(0x40, time >> 8);
}


void defultinthandle(int no, int code)
{
    uint32 cr2 = getcr2();
    switch (no) {
    case 14:
        if(cr2 < USEBASE)
            break;
        ptable *pdt = mappage(PROTABLE[curpid].pdt);
        
        switch (code & 0xf) {
        case 4: 
        case 6:
            if (pdt[getpagec(cr2)].P == 0) {
                pdt[getpagec(cr2)].base = getmpage();
                pdt[getpagec(cr2)].PAT = 0;
                pdt[getpagec(cr2)].D = 0;
                pdt[getpagec(cr2)].A = 0;
                pdt[getpagec(cr2)].PCD = 0;
                pdt[getpagec(cr2)].PWT = 0;
                pdt[getpagec(cr2)].U_S = 1;
                pdt[getpagec(cr2)].R_W = 1;
                pdt[getpagec(cr2)].P = 1;
            }
            
            ptable *pte = mappage(pdt[getpagec(cr2)].base);
            if (pte[getpagei(cr2)].P == 0) {
                pte[getpagei(cr2)].base = getmpage();
                pte[getpagei(cr2)].PAT = 0;
                pte[getpagei(cr2)].D = 0;
                pte[getpagei(cr2)].A = 0;
                pte[getpagei(cr2)].AVL = 0;
                pte[getpagei(cr2)].PCD = 0;
                pte[getpagei(cr2)].PWT = 0;
                pte[getpagei(cr2)].U_S = 1;
                pte[getpagei(cr2)].R_W = 1;
                pte[getpagei(cr2)].P = 1;
            }
            unmappage(pte);
            unmappage(pdt);
            invlpg(cr2);
            return;
        case 7:
            pte = mappage(pdt[getpagec(cr2)].base);
            if(pte[getpagei(cr2)].AVL == 1){
                devpage(pdt[getpagec(cr2)].base,getpagei(cr2));
                uint32 newpage=getmpage();
                pagecpy(newpage,pte[getpagei(cr2)].base);
                pte[getpagei(cr2)].base=newpage;
            }
            unmappage(pte);
            unmappage(pdt);
            invlpg(cr2);
            return;
        }
        unmappage(pdt);
    }
    printf("The int %d happened:%d!\n", no, code);
    while (1);
}


void init()
{
    int i;
    set8253(0xffff);
    Init8259();
    while((inp(0x64) & 1) == 1){        //清空键盘缓冲区
      inp(0x60);
    }
    for (i = 0; i < 255; i++) {
        Setinterrupt(i, defultinthandle);
    }
    Setinterrupt(0x20, TimerInitHandler);
    Setinterrupt(0x21, KeyBoadHandler);
    Setinterrupt(0x26, FloppyIntHandler);
    Setinterrupt(0x2e, HdIntHandler);
    Setinterrupt(0x2f, HdIntHandler);
    Setinterrupt(80, (void ( *)())syscall);

    outp(0x21, inp(0x21) & 0xfd);   //开启键盘中断
    outp(0x21, inp(0x21) & 0xfe);   //开启时钟中断
    outp(0x21, inp(0x21) & 0xfb);   //允许从片中断
    outp(0xa1, inp(0xa1) & 0xbf);   //开启硬盘中断
    outp(0xa1, inp(0xa1) & 0x7f);   //开启第二硬盘中断
    
    sti();
    putstring("Initing file system ...\n");
    initfs();
    putstring("File system inited.\n");
    cli();
    
    
    curpid = 0;                     //初始化进程0,即闲逛进程
    PROTABLE[curpid].status = ready;
    PROTABLE[curpid].waitresource=0;
    PROTABLE[curpid].pid = 0;
    PROTABLE[curpid].ppid = 0;

    GDT[UCODEI].base0_23 = 0;
    GDT[UCODEI].base24_31 = 0;
    GDT[UCODEI].limit0_15 = 0xffff;
    GDT[UCODEI].limit16_19 = 0xf;
    GDT[UCODEI].S = 1;
    GDT[UCODEI].D = 1;
    GDT[UCODEI].L = 0;
    GDT[UCODEI].P = 1;
    GDT[UCODEI].G = 1;
    GDT[UCODEI].DPL = 3;
    GDT[UCODEI].Type = DA_E | DA_WR;

    GDT[UDATAI].base0_23 = 0;
    GDT[UDATAI].base24_31 = 0;
    GDT[UDATAI].limit0_15 = 0xffff;
    GDT[UDATAI].limit16_19 = 0xf;
    GDT[UDATAI].S = 1;
    GDT[UDATAI].D = 1;
    GDT[UDATAI].L = 0;
    GDT[UDATAI].P = 1;
    GDT[UDATAI].G = 1;
    GDT[UDATAI].DPL = 3;
    GDT[UDATAI].Type = DA_WR;

    PROTABLE[curpid].reg.ss = UDATA_DT;
    PROTABLE[curpid].reg.oesp = 0xffffffff - KSL;
    PROTABLE[curpid].reg.cs = UCODE_DT;
    PROTABLE[curpid].reg.eip = (uint32)process0 + (uint32)USECODE;
    PROTABLE[curpid].reg.eflags = 0x1202;
    PROTABLE[curpid].reg.ds = UDATA_DT;
    PROTABLE[curpid].reg.es = UDATA_DT;
    PROTABLE[curpid].reg.fs = VGA_DT;
    PROTABLE[curpid].reg.gs = UDATA_DT;

    for (i = 1; i < MAX_PROCESS; ++i) {
        PROTABLE[i].status = unuse;
    }

    PROTABLE[curpid].file[0].isused = 1;             //for standard input
    PROTABLE[curpid].file[0].type = TTY;
    PROTABLE[curpid].file[1].isused = 1;             //for standard output
    PROTABLE[curpid].file[1].type = TTY;
    PROTABLE[curpid].file[2].isused = 1;             //for standard errer
    PROTABLE[curpid].file[2].type = TTY;
    for (i = 3; i < MAX_FD; i = i + 1) {
        PROTABLE[curpid].file[i].isused = 0;
    }
    PROTABLE[curpid].pdt = getmpage();

    ptable *pdt = (ptable *)(PROTABLE[curpid].pdt * PAGESIZE);
    for (i = 0; i < 1024; ++i) {
        pdt[i].P = 0;
    }
    pdt[0].base = getmpage();               //内核代码数据
    pdt[0].PAT = 0;
    pdt[0].A = 0;
    pdt[0].PCD = 0;
    pdt[0].PWT = 0;
    pdt[0].U_S = 0;
    pdt[0].R_W = 1;
    pdt[0].P = 1;

    ptable *pte = (ptable *)(pdt[0].base * PAGESIZE);
    for (i = 0; i < 1024; ++i) {
        pte[i].base = i;
        pte[i].PAT = 0;
        pte[i].D = 0;
        pte[i].A = 0;
        pte[i].PCD = 0;
        pte[i].PWT = 0;
        pte[i].U_S = 0;
        pte[i].R_W = 1;
        pte[i].P = 1;
    }
    
    pdt[1].base = getmpage();               //4M-5M
    pdt[1].PAT = 0;
    pdt[1].A = 0;
    pdt[1].PCD = 0;
    pdt[1].PWT = 0;
    pdt[1].U_S = 0;
    pdt[1].R_W = 1;
    pdt[1].P = 1;
    
    pte = (ptable *)(pdt[1].base * PAGESIZE);
    for (i = 0; i < 256; ++i) {
        pte[i].base = i+1024;
        pte[i].PAT = 0;
        pte[i].D = 0;
        pte[i].A = 0;
        pte[i].PCD = 0;
        pte[i].PWT = 0;
        pte[i].U_S = 0;
        pte[i].R_W = 1;
        pte[i].P = 1;
    }

    pdt[MAPINDEX].base = (uint32)TMPMAP/PAGESIZE;               //用于内核临时挂载页
    pdt[MAPINDEX].PAT = 0;
    pdt[MAPINDEX].A = 0;
    pdt[MAPINDEX].PCD = 0;
    pdt[MAPINDEX].PWT = 0;
    pdt[MAPINDEX].U_S = 0;
    pdt[MAPINDEX].R_W = 1;
    pdt[MAPINDEX].P = 1;
    
    memset(TMPMAP,0,PAGESIZE);
    
    pdt[USEPAGE].base = (uint32)getmpage();           //process0用户空间
    pdt[USEPAGE].PAT = 0;
    pdt[USEPAGE].A = 0;
    pdt[USEPAGE].PCD = 0;
    pdt[USEPAGE].PWT = 0;
    pdt[USEPAGE].U_S = 1;
    pdt[USEPAGE].R_W = 1;
    pdt[USEPAGE].P = 1;
    
    pte = (ptable *)(pdt[USEPAGE].base * PAGESIZE);
    pte[0].base=getmpage();
    pte[0].PAT = 0;
    pte[0].D = 0;
    pte[0].A = 0;
    pte[0].PCD = 0;
    pte[0].PWT = 0;
    pte[0].U_S = 1;
    pte[0].R_W = 1;
    pte[0].P = 1;
    pte[0].AVL=0;
    
    for (i = 1; i < 768; ++i) {
        pte[i].base = i-(USECODE-USEBASE)/PAGESIZE;
        pte[i].PAT = 0;
        pte[i].D = 0;
        pte[i].A = 0;
        pte[i].PCD = 0;
        pte[i].PWT = 0;
        pte[i].U_S = 1;
        pte[i].R_W = 1;
        pte[i].P = 1;
        pte[i].AVL=0;
    }

    pdt[USEENDP].base = (uint32)getmpage();           //process0堆栈
    pdt[USEENDP].PAT = 0;
    pdt[USEENDP].A = 0;
    pdt[USEENDP].PCD = 0;
    pdt[USEENDP].PWT = 0;
    pdt[USEENDP].U_S = 1;
    pdt[USEENDP].R_W = 1;
    pdt[USEENDP].P = 1;
    pte = (ptable *)(pdt[USEENDP].base * PAGESIZE);
    for (i = 0; i < 1024; ++i) {
        pte[i].P = 0;
    }
    pte[1022].base = (uint32)getmpage();                       //用户栈
    pte[1022].PAT = 0;
    pte[1022].D = 0;
    pte[1022].A = 0;
    pte[1022].PCD = 0;
    pte[1022].PWT = 0;
    pte[1022].U_S = 1;
    pte[1022].R_W = 1;
    pte[1022].P = 1;
    pte[1024].AVL=0;

    pte[1023].base = (uint32)getmpage();                       //系统栈(一个页面)
    pte[1023].PAT = 0;
    pte[1023].D = 0;
    pte[1023].A = 0;
    pte[1023].PCD = 0;
    pte[1023].PWT = 0;
    pte[1023].U_S = 0;
    pte[1023].R_W = 1;
    pte[1023].P = 1;

    PROTABLE[curpid].waitresource = 0;


    TSS.ss0 = KSTACK_DT;
    TSS.esp0 = 0xffffffff;


    GDT[TSSI].base0_23 = ((uint32)&TSS) & 0xffffff;
    GDT[TSSI].base24_31 = (uint32)&TSS >> 24;
    GDT[TSSI].limit0_15 = 104;
    GDT[TSSI].limit16_19 = 0;
    GDT[TSSI].S = 0;
    GDT[TSSI].D = 1;
    GDT[TSSI].L = 0;
    GDT[TSSI].P = 1;
    GDT[TSSI].G = 0;
    GDT[TSSI].DPL = 0;
    GDT[TSSI].Type = DA_ATSS;
    
    movetouse(&PROTABLE[curpid], pdt);
}


#include <unistd.h>

static char *const argv[]={"./exe.elf","hello",NULL};
static char *const env[]={"PATH=/",NULL};

void process0(void)
{
    if (fork() == 0) {
        execve("exe.elf", argv, env);
        while (1);
    } else {
        while (1);
    }
}

