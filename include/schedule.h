#ifndef __SCHEDULT_H__
#define __SCHEDULT_H__

#include <process.h>

void TimerInitHandler();
void schedule();
void switch_to(pid_t pid);
void do_switch_to(pid_t *last,process *next,uint32 pdt);


#endif