#ifndef __TYPE_H__
#define __TYPE_H__

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int   uint32;
typedef unsigned long long  uint64;


#ifndef NULL
#define NULL (void *)0
#endif

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;
typedef unsigned int off_t;
#endif


#ifndef BOOL
typedef unsigned char BOOL;
#define FALSE 0
#define TRUE  !FALSE
#endif

//typedef unsigned long int size_t;
//typedef unsigned long int off_t;
/*
0            未定义
1            可用286TSS
2            LDT
3            忙的286TSS
4            286调用门
5            任务门
6            286中断门
7            286陷阱门
8            未定义
9            可用386TSS
A            未定义
B            忙的386TSS
C            386调用门
D            未定义
E            386中断门
F            386陷阱门
*/

#define TaskGate    5
#define CallGate    C
#define IntrGate    E
#define TrapGate    F


typedef struct {
    uint16 offset0_15;
    uint16 tcss;               //target code segment selector
    uint8  paramc:5;           //only use in call gate
    uint8  :3;
    uint8  Type:5;
    uint8  DPL:2;
    uint8  P:1;                //是否存在
    uint16 offset16_31;
} __attribute__ ((packed)) gate;

/*非系统段中 type 字段 即S=1*/
#define DA_A    1           //是否被访问过
#define DA_WR   2           //数据段：是否可写         代码段：是否可读
#define DA_EDC  4           //数据段：向低扩展(很少用)  代码段：是否是一致性代码段
#define DA_E    8           //是否可执行

/*系统段中 type 字段 即S=0 */
#define DA_LDT     2
#define DA_ATSS    9
#define DA_BTSS    B


typedef struct {
    uint16 limit0_15;
    uint32 base0_23:24;
    uint32 Type:4;
    uint32 S:1;                //S=0 为系统段
    uint32 DPL:2;
    uint32 P:1;                //是否存在
    uint8 limit16_19:4;
    uint8 AVL:1;               //操作系统使用
    uint8 L:1;                 //用于x64
    uint8 D:1;                 //是否为32位
    uint8 G:1;                 //粒度 G=1 则limit 的单位为4K
    uint8 base24_31;

} __attribute__ ((packed)) ss;


typedef struct {
    uint32    back_link;  /*16 high bits zero */
    uint32    esp0;
    uint32    ss0;        /*16 high bits zero */
    uint32    esp1;
    uint32    ss1;        /*16 high bits zero */
    uint32    esp2;
    uint32    ss2;        /*16 high bits zero */
    uint32    cr3;
    uint32    eip;
    uint32    eflags;
    uint32    eax,ecx,edx,ebx;
    uint32    esp;
    uint32    ebp;
    uint32    esi;
    uint32    edi;
    uint32    es;         /*16 high bits zero */
    uint32    cs;         /*16 high bits zero */
    uint32    ss;         /*16 high bits zero */
    uint32    ds;         /*16 high bits zero */
    uint32    fs;         /*16 high bits zero */
    uint32    gs;
    uint32    ldt;
    uint32    trace_bitmap;/*trace 0, bitmap16-31 */
} __attribute__ ((packed)) tss_struct;


typedef struct {
    uint32 P:1;           //是否存在
    uint32 R_W:1;         //0为只读，1为读写(对系统特权级不起作用)
    uint32 U_S:1;         //0为系统级，1为用户级
    uint32 PWT:1;         //0为写回法，1为写直达法 (cache)
    uint32 PCD:1;         //是否禁止缓存(cache)
    uint32 A:1;           //是否被访问过
    uint32 D:1;           //ditty，对于页目录来说这位是无效的，会被处理器忽略
    uint32 PAT:1;         //是否指示一个4M的页，否则必须为0
    uint32 G:1;           //global 只在页表中有用,如果cr4.pge没有置位则会被忽略
    uint32 AVL:3;         //留给操作系统使用
    uint32 base:20;       //基地址
}__attribute__ ((packed)) ptable;

typedef struct{
    uint32 pagec;
    uint16 index;
    uint16 next;
}pageshrlist;


typedef long clock_t;
typedef long time_t;
typedef long pid_t;
typedef uint32 uid_t;
typedef uint32 gid_t;

struct timeval{
  time_t      tv_sec;
  time_t      tv_usec;
};

struct timezone{
    
};

/*  Get Process Times, P1003.1b-1993, p. 92 */
struct tms {
    clock_t tms_utime;      /* user time */
    clock_t tms_stime;      /* system time */
    clock_t tms_cutime;     /* user time, children */
    clock_t tms_cstime;     /* system time, children */
};

struct tm
{
  int   tm_sec;
  int   tm_min;
  int   tm_hour;
  int   tm_mday;
  int   tm_mon;
  int   tm_year;
  int   tm_wday;
  int   tm_yday;
  int   tm_isdst;
};

#endif
