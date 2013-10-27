#include <boot.h>
#include <chouryos.h>
#include <fcntl.h>
#include <floppy.h>
#include <hd.h>

int* __errno() {
    return (int *)12000;
}



void Setinterrupt(int into,void f()) {
    INTHER[into]=f;
}

void loadkernel(){
    Init8259();
    Setinterrupt(0x26,FloppyIntHandler);
    Setinterrupt(0x2e,HdIntHandler);

    outp(0x21,inp(0x21)&0xfb);      //允许从片中断
    outp(0xa1,inp(0xa1)&0xbf);      //开启硬盘中断
    sti();
    initfs();
    fileindex file;
    if(file_open(&file,"chouryos",O_RDWR)<0){
        putstring("Can't find kernel!\n");
    }else{
        file_read(&file,(void *)KernelLocation,0xffffffff);
        kernel();
    }
}
