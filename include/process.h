#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <type.h>
#include <file.h>

#define MAX_PROCESS 10


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



typedef struct{
    register_status reg;
    u32 pid;
    u32 ppid;
    u16 ldt;
    ss  cdt;
    ss  ddt;
    fileindex file[MAX_FD];
    u8 isused;
}__attribute__ ((packed))process;


#endif
