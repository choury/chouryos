#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <type.h>
#include <file.h>

#define MAX_PROCESS 10

typedef enum{
  unuse,ready,running,waiting,deaded
}pstatus;

typedef struct{
    uint32 gs;
    uint32 fs;
    uint32 es;
    uint32 ds;
    uint32 edi;
    uint32 esi;
    uint32 ebp;
    uint32 esp;
    uint32 ebx;
    uint32 edx;
    uint32 ecx;
    uint32 eax;
    uint32 eip;
    uint32 cs;
    uint32 eflags;
    uint32 oesp;
    uint32 ss;
}__attribute__ ((packed)) register_status;

//typedef u32 pid_t;

struct siglist{
    int sig;
    struct siglist *next;
};

typedef struct{
    register_status reg;
    pid_t pid;
    pid_t ppid;
    pstatus status;
    uint32 pde;                              //页目录表
    void *heap;                             //进程堆的起始地址
    filedes file[MAX_FD];                 //打开的文件
    int  ret;                               //返回值
    DEV  waitfor;
    struct siglist sighead;
}process;


#define PROTABLE    ((process *)0x100000)  //同asm中定义相同

struct pinfo{
    int argc;
    char **argv;
    char **env;
    int  errno;
    void *endp;
};

#define PINF       ((struct pinfo *)USEBASE)


void movetouse(process *,ptable *pdt);

#endif
