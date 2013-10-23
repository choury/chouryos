#include <hd.h>

static uint8 hdstats=0;

void HdInitHandler(){
    outp(0x20,0x20);
    hdstats=0xff;
}

static void WaitInit(){
    while(hdstats==0);
    hdstats=0;
}

static void sendcmd(uint8 cmd){
    while(inp(HD_STATUS) & BUSY_STAT);
    while(!(inp(HD_STATUS) & READY_STAT));
}

void resetHd(){
    int i;
    outp(HD_CMD,4);
    for(i=0;i<100;i++)
        __asm__("nop\n");
    
}
