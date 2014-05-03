#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <type.h>
#include <file.h>

#define MAX_PROCESS 10

typedef enum{
  unuse,ready,running,wait
}pstatus;

typedef struct{
    u32 gs;
    u32 fs;
    u32 es;
    u32 ds;
    u32 edi;
    u32 esi;
    u32 ebp;
    u32 esp;
    u32 ebx;
    u32 edx;
    u32 ecx;
    u32 eax;
    u32 eip;
    u32 cs;
    u32 eflags;
    u32 oesp;
    u32 ss;
}__attribute__ ((packed)) register_status;

//typedef u32 pid_t;

typedef struct{
    register_status reg;
    pid_t pid;
    pid_t ppid;
    pstatus status;
    uint32 pdt;                              //页目录表
//    void *base;                            //进程空间基址，也是cdt,ddt的基址
    void *heap;                             //进程堆的起始地址
    fileindex file[MAX_FD];                 //打开的文件
    u32 waitresource;                      //正在等待的资源数，当它为0时该进程状态应该为就绪
}process;


#define PROTABLE    ((process *)0x100000)  //同asm中定义相同

#endif
