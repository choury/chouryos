#include "boot.h"


void loadkernel(){
    Init8259();
    setinterrupt();
    sti();
    initfs();
    if(lopen("chouryos")<0){
        putstring("Can't find kernel!\n");
    }else{
        lread((void *)KernelLocation,0xffffffff);
    }
    kernel();
}
