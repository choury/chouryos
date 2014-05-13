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
    return &(PINF->errno);
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
    case 1:
        return;
    case 14:
        if(cr2 < USEBASE)
            break;
        ptable *pdt = mappage(PROTABLE[curpid].pdt);
        
        switch (code & 0xf) {
        case 0:
        case 2:
            if (pdt[getpagec(cr2)].P == 0) {
                SETPT(pdt[getpagec(cr2)],getmpage(),0);
            }
            
            ptable *pte = mappage(pdt[getpagec(cr2)].base);
            if (pte[getpagei(cr2)].P == 0) {
                SETPT(pte[getpagei(cr2)],getmpage(),0);
            }
            unmappage(pte);
            unmappage(pdt);
            invlpg(cr2);
            return;
        case 4: 
        case 6:
            if (pdt[getpagec(cr2)].P == 0) {
                SETPT(pdt[getpagec(cr2)],getmpage(),1);
            }
            
            pte = mappage(pdt[getpagec(cr2)].base);
            if (pte[getpagei(cr2)].P == 0) {
                SETPT(pte[getpagei(cr2)],getmpage(),1);
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
    register_status *prs = (register_status *)(0xffffffff - sizeof(register_status));
    printf("[%d:0x%X] The int %d happened:%d!\n",curpid,prs->eip, no, code);
    sti();
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
    printf("Initing file system ...\n");
    initfs();
    printf("File system inited.\n");
    cli();
    
    
    curpid = 0;                     //初始化进程0,即闲逛进程
    PROTABLE[curpid].status = ready;
    PROTABLE[curpid].pid = 0;
    PROTABLE[curpid].ppid = 0;
    PROTABLE[curpid].sighead.next=NULL;

    
    SETSS(GDT[MCODEI],0,0xfffff,DA_E | DA_WR,1,0,1,1,0,1,1);  //设置内核模块的代码段
    SETSS(GDT[MDATAI],0,0xfffff,DA_WR,1,0,1,1,0,1,1);         //设置内核模块的数据段
    
    SETSS(GDT[UCODEI],0,0xfffff,DA_E | DA_WR,3,0,1,1,0,1,1);  //设置用户进程的代码段
    SETSS(GDT[UDATAI],0,0xfffff,DA_WR,3,0,1,1,0,1,1);         //用户进程的数据段
    

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
    
    
    SETPT(pdt[0],getmpage(),0);
    ptable *pte = (ptable *)(pdt[0].base * PAGESIZE);
    for (i = 0; i < 1024; ++i) {
        SETPT(pte[i],i,0);                         //内核代码数据

    }
    pte[0].R_W=0;                                  //idt和gdt不可写
    
    SETPT(pdt[1],getmpage(),0);                    //4M-5M

    
    pte = (ptable *)(pdt[1].base * PAGESIZE);
    for (i = 0; i < 256; ++i) {
        SETPT(pte[i],i+1024,0);
    }

    SETPT(pdt[MAPINDEX],(uint32)TMPMAP/PAGESIZE,0); //用于内核临时挂载页
    memset(TMPMAP,0,PAGESIZE);
     
    SETPT(pdt[USEPAGE],getmpage(),1);               //process0用户空间
    
    pte = (ptable *)(pdt[USEPAGE].base * PAGESIZE);
    SETPT(pte[0],getmpage(),1);
    
    for (i = 1; i < 768; ++i) {
        SETPT(pte[i],i-(USECODE-USEBASE)/PAGESIZE,1);
    }
    
    SETPT(pdt[USEENDP],getmpage(),1);               //process0堆栈
    pte = (ptable *)(pdt[USEENDP].base * PAGESIZE);
    for (i = 0; i < 1024; ++i) {
        pte[i].P = 0;
    }

    SETPT(pte[1023],getmpage(),0);                  //系统栈(一个页面)

    //设置TSS
    TSS.ss0 = KSTACK_DT;
    TSS.esp0 = 0xffffffff;
//                                       dpl,avl,s,d,l,p,g
//                                          \  | | | | | |
    SETSS(GDT[TSSI],(uint32)&TSS,104,DA_ATSS,0,0,0,1,0,1,0);
    
    movetouse(&PROTABLE[curpid], pdt);
}


#include <unistd.h>

static char *const argv[]={"./init","hello",NULL};
static char *const env[]={"PATH=/",NULL};

void process0(void)
{
    if (fork() == 0) {
        execve("init", argv, env);
        write(STDOUT_FILENO,"init not fount!\n",16);
        while(1);
    } else {
        while (1){
//            write(STDOUT_FILENO,"0",1);
        }
    }
}

