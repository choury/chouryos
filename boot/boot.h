#ifndef __BOOT_H__
#define __BOOT_H__


#define uint8  unsigned char
#define uint16 unsigned short
#define uint32 unsigned int

#define u8      uint8
#define u16     uint16
#define u32     uint32


/* we use this so that we can do without the ctype library */
#define isdigit(c) ((c) >= '0' && (c) <= '9')
#define isxdigit(c) (((c) >= '0' && (c) <= '9') ||\
                     ((c) >= 'A' && (c) <= 'F') ||\
                     ((c) >= 'a' && (c) <= 'f'))
#define islower(c)  ((c)>='a' && (c)<='z')
#define toupper(c)  ((c)-32)

#define Floppybuff      (unsigned char *)4096           //if you changed this,you should change the addr in copyfloppydata also


void sti();
void cli();

void outp(unsigned int port,unsigned int data);
void outpw(unsigned int port,unsigned int data);
unsigned char inp(unsigned int port);
void copyfloppydata(unsigned char *buffer);

int printf(const char *fmt,...);
int putchar(int);
int puts(const char*);

#define STATUS_REG_A            0x03f0 /*PS2 SYSTEMS*/
#define STATUS_REG_B            0x03f1 /*PS2 SYSTEMS*/
#define DIGITAL_OUTPUT_REG      0x03f2
#define MAIN_STATUS_REG         0x03f4
#define DATA_RATE_SELECT_REG    0x03f4 /*PS2 SYSTEMS*/
#define DATA_REGISTER           0x03f5
#define DIGITAL_INPUT_REG       0x03f7 /*AT SYSTEMS*/
#define CONFIG_CONTROL_REG      0x03f7 /*AT SYSTEMS*/

#define sti()     __asm__("sti\n")
#define cli()     __asm__("cli\n")



/*controller commands*/
#define FIX_DRIVE_DATA          0x03
#define CHECK_DRIVE_STATUS      0x04
#define CALIBRATE_DRIVE         0x07
#define CHECK_INTERRUPT_STATUS  0x08
#define FORMAT_TRACK            0x4D
#define READ_SECTOR             0xC6
#define READ_DELETE_SECTOR      0xCC
#define READ_SECTOR_ID          0x4A
#define READ_TRACK              0x42
#define SEEK_TRACK              0x0F
#define WRITE_SECTOR            0xC5
#define WRITE_DELETE_SECTOR     0xC9

/* Bits of main status register */
#define STATUS_BUSYMASK 0x0F        /* drive busy mask */
#define STATUS_BUSY 0x10        /* FDC busy */
#define STATUS_DMA  0x20        /* 0- DMA mode */
#define STATUS_DIR  0x40        /* 0- cpu->fdc */
#define STATUS_READY    0x80        /* Data reg ready */


void reset_floppy_controller(char drive);
void calibrate_drive(char drive);
void configure_drive(char drive) ;
void send_byte(unsigned char command) ;
unsigned char get_byte();
void seek_track(unsigned char head,unsigned char track,unsigned char drive);
void rw_sector(unsigned char drive,unsigned int block,unsigned char * buffer,unsigned char command);
void readfloppyA(unsigned int block,unsigned char *buffer);
void writefloppyA(unsigned int block,unsigned char *buffer);


extern uint8   BUFFER_FAT[512];


typedef struct
{
    uint8         BS_jmpBoot[3];
    uint8         BS_OEMName[8];
    uint16        BPB_BytesPerSec;
    uint8         BPB_SecPerClus;
    uint16        BPB_RsvdSecCnt;
    uint8         BPB_NumFATs;
    uint16        BPB_RootEntCnt;
    uint16        BPB_TotSec16;
    uint8         BPB_Media;
    uint16        BPB_FATSz16;
    uint16        BPB_SecPerTrk;
    uint16        BPB_NumHeads;
    uint32        BPB_HiddSec;
    uint32        BPB_TotSec32;
    uint8         BS_DrvNum;
    uint8         BS_Reservedl;
    uint8         BS_BootSig;
    uint32        BS_VolID;
    uint8         BS_VolLab[11];
    uint8         BS_FilSysType[8];
    uint8         ExecutableCode[448];
    uint8         ExecutableMarker[2];
} __attribute__ ((packed)) FAT_BPB;

typedef struct
{
    uint16        Start;
    uint32        Size;
} __attribute__ ((packed)) FILE_POSIT;

typedef struct
{
    uint8         FileName[11];
    uint8         FileAttrib;
    uint8         UnUsed[10];
    uint8         FileUpdateTime[2];
    uint8         FileUpdateData[2];
    FILE_POSIT    FilePosit;
} __attribute__ ((packed)) DIR;

void FAT_Init(void);

void     ReadBlock        (uint32 LBA);
void     WriteBlock       (uint32 LBA);
uint32 DirStartSec(void);
uint32 DataStartSec(void);



typedef struct{
    uint8  isused;
    uint16 indexno;
    uint32 offset;
    uint16 startnode;
    uint16 curnode;
    uint32 length;
}__attribute__ ((packed)) fileindex;


#endif
