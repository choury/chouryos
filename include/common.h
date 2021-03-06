#ifndef __COMMON_H__
#define __COMMON_H__


#include <type.h>
#include <time.h>

#define MAXARGLEN       1024                        //最长参数长度

//#define IDT     ((gate *)0)           不需要它了
#define GDT     ((ss *)0x800)
#define TSS     (*(tss_struct *)0x1200)


#define KCODEI          1                           //内核代码段索引
#define KDATAI          2                           //内核数据段
#define VGAI            3                           //显存(非扁平模式) 存在fs中
#define TSSI            4                           //tss段
#define MCODEI          5                           //用户代码段
#define MDATAI          6                           //用户数据段
#define UCODEI          7                           //用户代码段
#define UDATAI          8                           //用户数据段


#define MCODE_DT          ((MCODEI<<3)|1)          //内核模块cs
#define MDATA_DT          ((MDATAI<<3)|1)          //内核模块ds,es,gs
#define UCODE_DT          ((UCODEI<<3)|3)          //用户cs
#define UDATA_DT          ((UDATAI<<3)|3)          //用户ds,es,gs
#define KSTACK_DT         (KDATAI<<3)              //用户内核栈
#define VGA_DT            (VGAI<<3)

#define KSL             PAGESIZE                      //每个进程内核栈的大小

#define charbuff ((uint16 *)0xB8000)

#define INTHER  ((void (**)())0x2800)
#define MTMAP   ((uint8 *)0x2c00)
#define TMPMAP  ((ptable *)0x3000)

#define KHEAP   ((void *) 0x10000)
#define MMAP    ((uint8 *)0x300000)
#define PSL   ((pageshrlist*)0x400000)
#define USEBASE 0x40000000                          //1G以上为用户空间
#define USECODE (USEBASE+PAGESIZE)                  //进程代码开始地址

#define MAX_SHRLC (0x100000/sizeof(pageshrlist))

#define USEPAGE ((uint32)USEBASE>>22)
#define USEENDP 1023
#define ENDPAGE (USEENDP+1)

#define MAPINDEX USEPAGE-1


extern pid_t curpid;

#define sti()     asm volatile ("sti\n")
#define cli()     asm volatile ("cli\n")
#define nop()     asm volatile ("nop\n")
#define lldt(x)   asm volatile ("lldt %0" : :"r"(x))
#define ldcr3(x)  asm volatile ("movl %0,%%cr3": :"r"(x))
#define invlpg(x) asm volatile ("invlpg (%0)" : :"r"(x))
#define invlapg()\
do {unsigned long tmpreg; \
    asm volatile ("movl %%cr3,%0\nmovl %0,%%cr3":"=r" (tmpreg): :"memory");\
    }\
while (0)
 
#define MAX(x,y)    ((x)>(y)?(x):(y))
#define MIN(x,y)    ((x)<(y)?(x):(y))
    
    
static inline uint32 getcr2(void)
{
    uint32 val;
    asm volatile ( "mov %%cr2, %0" : "=r"(val) );
    return val;
}

void Init8259();
void outp(unsigned int port,unsigned int data);
void outpw(unsigned int port,unsigned int data);
void outpd(unsigned int port,unsigned int data);
unsigned char inp(unsigned int port);
unsigned short inpw(unsigned int port);
unsigned int inpd(unsigned int port);
void inpn(unsigned int port,void *buff,int n);
void inpwn(unsigned int port,void *buff,int n);
void inpdn(unsigned int port,void *buff,int n);


void process0(void);


int printf(const char *,...);

time_t kernel_mktime(
    unsigned int year, unsigned int mon,
    unsigned int day, unsigned int hour,
    unsigned int min, unsigned int sec);

time_t kernel_getnowtime();
void time_to_tm(time_t totalsecs, struct tm *result);

#endif
