#ifndef __BOOT_H__
#define __BOOT_H__

void Init8259();



#define KernelLocation  0x200000

#define kernel          ((void (*)())KernelLocation)


#endif
