#include "chouryos.h"


#ifndef __FAT_H__
#define __FAT_H__


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
    uint8         Name[11];
    uint8         Attrib;
    uint8         UnUsed;
    uint8         CreateTimems;
    uint16        Create2Second:5;
    uint16        CreateMinute:6;
    uint16        CreateHour:5;
    uint16        CreateDay:5;
    uint16        CreateMonth:4;
    uint16        CreateYear:7;
    uint16        AccessDay:5;
    uint16        AccessMonth:4;
    uint16        AccessYear:7;
    uint16        Starth;
    uint16        Update2Second:5;
    uint16        UpdateMinute:6;
    uint16        UpdateHour:5;
    uint16        UpdateDay:5;
    uint16        UpdateMonth:4;
    uint16        UpdateYear:7;
    uint16        Startl;
    uint32        Length;
} __attribute__ ((packed)) DIR;

typedef struct{
    uint8         Index;
    uint16        Name1[5];
    uint8         Attrib;
    uint8         UnUsed;
    uint8         Sum;
    uint16        Name2[6];
    uint16        Start;
    uint16        Name3[2];
}__attribute__ ((packed)) LFN;


void FAT_Init(void);

void     ReadBlock        (uint32 LBA);
void     WriteBlock       (uint32 LBA);
uint32 DirStartSec(void);
uint32 DataStartSec(void);

int getnextnode(uint32 node);
int getprenode(uint32 node);
int getblanknode(uint32 node);
int releasenode(u32 node,BOOL flag);      //释放node以后所有节点，如果flag为TRUE 则node也被释放

u8 getchecksum (const unsigned char Name[11]);  // 长文件名的校验和

#endif
