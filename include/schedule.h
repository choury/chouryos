#ifndef __SCHEDULT_H__
#define __SCHEDULT_H__


void TimerInitHandler();
void schedule();
void switch_to(u32 pid);
void do_switch_to(void *last,void *next,u16 ldt);


#endif