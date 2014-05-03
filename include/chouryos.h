#ifndef __CHOURYOS_H__
#define __CHOURYOS_H__

#include <process.h>
#include <type.h>
#include <sys/types.h>


//#define IDT     ((gate *)0)           不需要它了
#define GDT     ((ss *)0x800)
#define TSS     (*(tss_struct *)0x1200)


#define KCODEI          1                           //内核代码段索引
#define KDATAI          2                           //内核数据段
#define VGAI            3                           //显存(非扁平模式) 存在fs中
#define TSSI            4                           //tss段
#define UCODEI          5                           //用户代码段
#define UDATAI          6                           //用户数据段

#define UCODE_DT          ((UCODEI<<3)|3)          //用户cs
#define UDATA_DT          ((UDATAI<<3)|3)          //用户ds,es,gs
#define KSTACK_DT         (KDATAI<<3)              //用户内核栈
#define VGA_DT            (VGAI<<3)

#define KSL             0x1000              //内核栈的大小

#define charbuff ((u16 *)0xB8000)

#define INTHER  ((void (**)())0x2800)

//#define KPDE  ((ptable*)0x3000)
//#define KPTE  ((ptable*)0x4000)
#define KHEAP   ((void *) 0x10000)
#define MMAP    ((uint8 *)0x300000)
#define KINDEX  ((ptable *)0x380000)
#define USEBASE 0x40000000  //1G以上为用户空间
#define USEPAGE (USEBASE>>22)
#define USEENDP 1023
#define ENDPAGE (USEENDP+1)

#define CPYDEST   1018
#define CPYSRC    1019
#define TMPINDEX0 1020
#define TMPINDEX1 1021
#define TMPINDEX2 1022
#define TMPINDEX3 1023

extern u32 curpid;

#define sti()     __asm__("sti\n")
#define cli()     __asm__("cli\n")
#define nop()     __asm__("nop\n")
#define lldt(x)   __asm__ ("lldt %0" : :"r"(x))
#define invlpg(x) __asm__("invlpg (%0)" : :"r"(x))

void outp(unsigned int port,unsigned int data);
void outpw(unsigned int port,unsigned int data);
void outpd(unsigned int port,unsigned int data);
unsigned char inp(unsigned int port);
unsigned short inpw(unsigned int port);
unsigned int inpd(unsigned int port);
void inpn(unsigned int port,void *buff,int n);
void inpwn(unsigned int port,void *buff,int n);
void inpdn(unsigned int port,void *buff,int n);

void movetouse(process *,ptable *pdt);
void process0(void);



int putstring(const char* s);
int printf(const char *,...);

time_t kernel_mktime(
    unsigned int year, unsigned int mon,
    unsigned int day, unsigned int hour,
    unsigned int min, unsigned int sec);

time_t kernel_getnowtime();
void time_to_tm(time_t totalsecs, struct tm *result);

#endif
