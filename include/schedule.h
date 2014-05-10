#ifndef __SCHEDULT_H__
#define __SCHEDULT_H__

#include <process.h>

void TimerInitHandler();
void schedule();
void block(pid_t pid,DEV waitfor);
void unblock(pid_t pid);
void switch_to(pid_t pid);
void do_switch_to(pid_t *last,process *next,uint32 pdt);


#endif