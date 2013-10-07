#include <sys/types.h>

#ifndef __TYPE_H__
#define __TYPE_H__

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int   uint32;

typedef uint8   u8;
typedef uint16 u16;
typedef uint32 u32;


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
    u16 offset0_15;
    u16 tcss;               //target code segment selector
    u8  paramc:5;           //only use in call gate
    u8  :3;
    u8  Type:5;
    u8  DPL:2;
    u8  P:1;                //是否存在
    u16 offset16_31;
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
    u16 limit0_15;
    u32 base0_23:24;
    u32 Type:4;
    u32 S:1;                //S=0 为系统段
    u32 DPL:2;
    u32 P:1;                //是否存在
    u8 limit16_19:4;
    u8 AVL:1;               //操作系统使用
    u8 L:1;                 //用于x64
    u8 D:1;                 //是否为32位
    u8 G:1;                 //粒度 G=1 则limit 的单位为4K
    u8 base24_31;

} __attribute__ ((packed)) ss;


typedef struct {
    u32    back_link;  /*16 high bits zero */
    u32    esp0;
    u32    ss0;        /*16 high bits zero */
    u32    esp1;
    u32    ss1;        /*16 high bits zero */
    u32    esp2;
    u32    ss2;        /*16 high bits zero */
    u32    cr3;
    u32    eip;
    u32    eflags;
    u32    eax,ecx,edx,ebx;
    u32    esp;
    u32    ebp;
    u32    esi;
    u32    edi;
    u32    es;         /*16 high bits zero */
    u32    cs;         /*16 high bits zero */
    u32    ss;         /*16 high bits zero */
    u32    ds;         /*16 high bits zero */
    u32    fs;         /*16 high bits zero */
    u32    gs;
    u32    ldt;
    u32    trace_bitmap;/*trace 0, bitmap16-31 */
} __attribute__ ((packed)) tss_struct;



#endif
