#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <type.h>
#include <file.h>

#define MAX_PROCESS 10

typedef struct{
    u32 eax;
    u32 ebx;
    u32 ecx;
    u32 edx;
    u32 esi;
    u32 edi;
    u32 esp;
    u32 ebp;
    u32 eip;
    u32 eflags;
    u16 cs;
    u16 ds;
    u16 ss;
    u16 es;
    u16 fs;
    u16 gs;
}__attribute__ ((packed)) register_status;

typedef struct{
    register_status regs;
    u32 pid;
    u32 ppid;
    u16 ldt;
    ss  cdt;
    ss  ddt;
    fileindex file[MAX_FD];
    u8 isused;
}__attribute__ ((packed))process;


#endif
