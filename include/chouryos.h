#ifndef __CHOURYOS_H__
#define __CHOURYOS_H__

#include <process.h>
#include <type.h>


#ifndef NULL
#define NULL (void *)0
#endif


#define Floppybuff      (unsigned char *)4096


#define IDT     ((gate *)0)
#define GDT     ((ss *)2048)

#define charbuff ((u16 *)0xB8000)


#define INTHER  ((void (**)())10240)

#define line    (*(short *) 4608)
#define colume  (*(short *) 4610)

#define PROTABLE ((process *)0x100000)
#define CURPID  (*(u32 *)12000)

#define sti()     __asm__("sti\n")
#define cli()     __asm__("cli\n")

void outp(unsigned int port,unsigned int data);
void outpw(unsigned int port,unsigned int data);
unsigned char inp(unsigned int port);


void setpoint(int x,int y,int color);
void setinterrupt(int into,void f());






#endif
