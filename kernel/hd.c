#include <hd.h>
#include <chouryos.h>

static uint8 hdstats;
static uint8 drive;
static uint16 nsector;
static uint32 sector;
static uint8 head;

void HdIntHandler() {
    outp(0x20,0x20);
    outp(0xa0,0x20);
    hdstats=0xff;
}

static void WaitInit() {
    while(hdstats==0);
    hdstats=0;
}

static void sendcmd(uint8 cmd) {
    while(inp(HD_STATUS) & BUSY_STAT);
    while(!(inp(HD_STATUS) & READY_STAT));
//    outp(HD_CMD,HdInfo[0].ctl);
//    outp(HD_PRECOMP,HdInfo[0].wpcom>>2);
    outp(HD_NSECTOR1,nsector>>8);               //读写扇区数，高8位
    outp(HD_LBA3,sector>>24);
    outp(HD_LBA4,0);
    outp(HD_LBA5,0);
    outp(HD_NSECTOR,nsector);                   //读写扇区数，低8位
    outp(HD_LBA0,sector);                       //起始扇区
    outp(HD_LBA1,sector>>8);
    outp(HD_LBA2,sector>>16);
    outp(HD_EVSEL,0xE0 | (drive<<4) | head );  //驱动器号，磁头号
    outp(HD_COMMAND,cmd);
}

static int win_result(void)
{
    int i=inp(HD_STATUS);

    if ((i & (BUSY_STAT | READY_STAT | WRERR_STAT | SEEK_STAT | ERR_STAT))
        == (READY_STAT | SEEK_STAT))
        return(0); /* ok */
    if (i&1) i=inp(HD_ERROR);
    return 1;
}

void resetHd(int driver) {
    hdstats=0;
    head=0;
    outp(HD_CMD,4);
    outp(HD_CMD,0);
    inp(HD_CMD);
    inp(HD_CMD);
    inp(HD_CMD);
    inp(HD_CMD);
}



void readHd(int sec,int n,void *buff) {
    sector=sec;
    nsector=n;
    sendcmd(WIN_READ);
    while(nsector--) {
        WaitInit();
        if (win_result()) {
            putstring("HardDisk Read Error!\n");
        }
        inpwn(HD_DATA,buff,256);
        buff+=512;
    }
    while(inp(HD_STATUS) & BUSY_STAT);
}

void writeHd(int sec,int nsec,void *buff) {
}
