#include <hd.h>
#include <chouryos.h>

static uint8 hdstats=0;
static uint8 nsector,sector,head,drive;
static uint16 cyl;

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
    outp(HD_CMD,HdInfo[0].ctl);
    outp(HD_PRECOMP,HdInfo[0].wpcom>>2);
    outp(HD_NSECTOR,nsector);               //对写扇区数
    outp(HD_SECTOR,sector);                 //起始扇区
    outp(HD_LCYL,cyl&0xff);                 //起始柱面
    outp(HD_HCYL,cyl>>8);
    outp(HD_CURRENT,0xA0 | (drive<<4) | head);  //驱动器号，磁头号
    outp(HD_COMMAND,cmd);
}

static int win_result(void)
{
    int i=inp(HD_STATUS);

    if ((i & (BUSY_STAT | READY_STAT | WRERR_STAT | SEEK_STAT | ERR_STAT))
        == (READY_STAT | SEEK_STAT))
        return(0); /* ok */
    if (i&1) i=inp(HD_ERROR);
    return (1);
}

void resetHd(int driver) {
    int i;
    outp(HD_CMD,4);
    for(i=0; i<100; i++)
        __asm__("nop\n");
    outp(HD_CMD,HdInfo[0].ctl & 0x0f );
    drive=driver;
    nsector=HdInfo[0].spt;
    sector=HdInfo[0].spt;
    head=HdInfo[0].heads-1;
    cyl=HdInfo[0].cyl;
    sendcmd(WIN_SPECIFY);
    WaitInit();
}



void readHd(int sec,int n,void *buff) {
    sector=(sec%HdInfo[0].spt)+1;
    cyl=(sec/HdInfo[0].spt)/HdInfo[0].heads;
    head=(sec/HdInfo[0].spt)%HdInfo[0].heads;
    nsector=n;
    sendcmd(WIN_SEEK);
    WaitInit();
    sendcmd(WIN_READ);
    while(nsector--) {
        WaitInit();
        if (win_result()) {
            putstring("Read error!\n");
        }
        inpwn(HD_DATA,buff,256);
        buff+=512;
    }
    while(inp(HD_STATUS) & BUSY_STAT);
    if(inp(HD_SECTOR) != ((sec+n)%HdInfo[0].spt)+1 ||
            ((inp(HD_HCYL)<<8)|inp(HD_LCYL))!= ((sec+n)/HdInfo[0].spt)/HdInfo[0].heads ||
            (inp(HD_CURRENT)&0xf) != ((sec+n)/HdInfo[0].spt)%HdInfo[0].heads )
        putstring("Read Hard Disk Error!\n");
}

void writeHd(int sec,int nsec,void *buff) {
}
