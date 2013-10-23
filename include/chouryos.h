#ifndef __CHOURYOS_H__
#define __CHOURYOS_H__

#include <process.h>
#include <type.h>
#include <graphy.h>
#include <sys/types.h>
#include <hd.h>

#define Floppybuff      ((unsigned char *)4096)


#define IDT     ((gate *)0)
#define GDT     ((ss *)0x800)
#define TSS     (*(tss_struct *)0x1200)
#define HdInfo  ((Hdinfo *)0x1300)

#define KERNELCODE_DT  1
#define KERNELDATA_DT  2
#define VGA_DT         3
#define TSS_DT         4
#define LDT_START      5

#define charbuff ((u16 *)0xB8000)

#define INTHER  ((void (**)())0x2800)

#define PROTABLE    ((process *)0x100000)

extern u32 curpid;

#define sti()     __asm__("sti\n")
#define cli()     __asm__("cli\n")
#define nop()     __asm__("nop\n")
#define lldt(x)   __asm__ ("lldt %0" : :"r"(x));

void outp(unsigned int port,unsigned int data);
void outpw(unsigned int port,unsigned int data);
unsigned char inp(unsigned int port);
void movetouse(process *);
void process0(void);

int setpoint(int x,int y,RGB_t color);
void setinterrupt(int into,void f());

int putstring(const char* s);

time_t kernel_mktime(
    unsigned int year, unsigned int mon,
    unsigned int day, unsigned int hour,
    unsigned int min, unsigned int sec);

time_t kernel_getnowtime();
void time_to_tm(time_t totalsecs, struct tm *result);
#endif
