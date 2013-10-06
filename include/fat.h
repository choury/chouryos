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

#endif
