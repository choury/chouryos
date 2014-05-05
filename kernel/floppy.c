#include <stdio.h>
#include <string.h>
#include <chouryos.h>
#include <floppy.h>


//#define  drive 0

static uint8 floppystatus;

static unsigned char st0,current_track;


void FloppyIntHandler(){
    outp(0x20,0x20);
    floppystatus=0xff;
}

static int waitinterrupt() {
    while(floppystatus==0);
    floppystatus=0;
    return 0;
}

unsigned char get_byte(){
    int counter;
    unsigned char status=0;
    for(counter = 0 ; counter < 10000 ; counter++) {
        status = inp(MAIN_STATUS_REG) & (STATUS_READY | STATUS_DIR );
        if ((status == STATUS_READY) | STATUS_DIR) {
            return inp(DATA_REGISTER);
        }
    }
    putstring("Unable to get byte from FDC!\n");
    return 0;
}


void send_byte(unsigned char command) {
    int counter;
    unsigned char status;
    for(counter = 0 ; counter < 10000 ; counter++) {
        status = inp(MAIN_STATUS_REG) & (STATUS_READY | STATUS_DIR );
        if (status == STATUS_READY) {
            outp(DATA_REGISTER,command);
            return;
        }
    }
    putstring("Unable to send byte to FDC!\n");


}


static int result(void)
{
    send_byte(CHECK_INTERRUPT_STATUS);
    st0=get_byte();
    current_track=get_byte();
    return 0;
}


void configure_drive(char drive) {
    send_byte(FIX_DRIVE_DATA);
    send_byte(0xcf);         /* hut etc */
    send_byte(6);            /* Head load time =6ms, DMA */
    return;
}


void calibrate_drive(char drive) {
    outp(DIGITAL_OUTPUT_REG,0x0c | 1<<(4+drive));
    send_byte(CALIBRATE_DRIVE); /*Calibrate drive*/
    send_byte(drive);
    waitinterrupt();
    result();
    return;
}


/*base is the address of the controller, either PRIMARY of SECONDARY
drive is the drive on the controller to be reset - 0 is drive A (fd0),1 is drive B (fd1)*/

void reset_floppy_controller(char drive) {
    cli();
    floppystatus=0;
    outp(DIGITAL_OUTPUT_REG,00); /*disable controller*/
    int i;
    for(i=0; i<200; i++)
        __asm__("nop\n");
    outp(DIGITAL_OUTPUT_REG,0x0c); /*enable controller*/
    sti();
    waitinterrupt();
    result();
    outp(CONFIG_CONTROL_REG,0);

    configure_drive(drive);
    calibrate_drive(drive);
    return;
}



void setup_DMA(unsigned char *  buffer,unsigned char command) {
    long addr = (long) buffer;          // 当前请求项缓冲区所处内存中位置(地址)。
    cli();
    /* mask DMA 2 */
    outp(10,6);                        // 单通道屏蔽寄存器端口为 10。位 0-1 指定 DMA 通道(0--3),位 2:1 表示屏蔽,0 表示允许请求。
    /* output command byte. I don't know why, but everyone (minix, */
    /* sanches & canton) output this twice, first to 12 then to 11 */

    outp(0x0c,0xff);
    /* 8 low bits of addr */
    /* 地址低 0-7 位 */
    outp(4,addr);                       // 向 DMA 通道 2 写入基/当前地址寄存器(端口 4)。
    addr >>= 8;
    /* bits 8-15 of addr */
    outp(4,addr);
    addr >>= 8;
    outp(0x0c,0xff);
    /* bits 16-19 of addr */
    outp(0x81,addr);                    // DMA 只可以在 1M 内存空间内寻址,其高 16-19 位地址需放入页面寄存器(端口 0x81)。
    /* low 8 bits of count-1 (512-1=0x1ff) */
    outp(5,0xff);                       // 向 DMA 通道 2 写入基/当前字节计数器值(端口 5)。
    /* high 8 bits of count-1 */
    outp(5,1);                          // 一次共传输 512 字节(一个扇区)。
    /* activate DMA 2 */
    outp(0x0b,command);
    outp(10,2);                         // 复位对 DMA 通道 2 的屏蔽,开放 DMA2 请求 DREQ 信号。
    sti();
}





/*Note:
sector,head and cylinder are the CHS value for the data
required drive is the drive on the controller to be reset - 0 is drive A (fd0),1 is drive B (fd1)*/


void seek_track(unsigned char head,unsigned char track,unsigned char drive) {
    send_byte(SEEK_TRACK);               // 发送磁头寻道命令。
    send_byte(head<<2 | drive);          //发送参数:磁头号+当前软驱号。
    send_byte(track);                    // 发送参数:磁道号。
    waitinterrupt();
    result();
    if(current_track!=track){
        putstring("Seek failed\n");
        seek_track(head,track,drive);
    }
    return;
}

/*drive: 0 for A:,1 for B: etc
 * buffer's address should be slower than 1M
 *command : 0x46 for read,0x4a for write
 */

void rw_sector(unsigned char drive,unsigned int block,unsigned char * buffer,unsigned char command) {
    unsigned char sector,head,track;
    sector = block % 18;                // 起始扇区对每磁道扇区数取模,得磁道上扇区号。
    block /= 18;                        // 起始扇区对每磁道扇区数取整,得起始磁道数。
    head = block % 2;                   // 起始磁道数对磁头数取模,得操作的磁头号。
    track = block / 2;                  // 起始磁道数对磁头数取整,得操作的磁道号。

    seek_track(head,track,drive);
    outp (0x3F7, 0);
    setup_DMA(buffer,command);
    switch(command){
    case 0x46:
        send_byte(READ_SECTOR);
        break;
    case 0x4a:
        send_byte(WRITE_SECTOR);
        break;
    default:
        putstring("Wrong Floppy command!\n");
        return;
    }
    send_byte(head<<2|drive);
    send_byte(track);
    send_byte(head);
    send_byte(sector);
    send_byte(2);            /*sector size = 128*2^size*/
    send_byte(18);           /*last sector*/
    send_byte(0x1b);         /*27 default gap3 value*/
    send_byte(0xff);         /*default value for data length*/
    waitinterrupt();
    int i;
    for(i=0;i<7;++i){
        get_byte();
    }
    return;
}





void readfloppyA(unsigned int block,void *buffer) {
    rw_sector(0,block+1,Floppybuff,0x46);
    memcpy(buffer,Floppybuff,512);
}


void writefloppyA(unsigned int block, void* buffer){
    memcpy(Floppybuff,buffer,512);
    rw_sector(0,block+1,Floppybuff,0x4a);
}
