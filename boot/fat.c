#include "boot.h"


uint8           BUFFER_FAT[512];      //512字节的缓冲区

static uint8           BPB_SecPerClus;
static uint16          BPB_RsvdSecCnt;
static uint8           BPB_NumFATs;
static uint16          BPB_RootEntCnt;
static uint16          BPB_TotSec16;
static uint16          BPB_FATSz16;
static uint32          BPB_HiddSec;


static void     ReadBPB          (void);

void FAT_Init(void)
{
    ReadBPB();
}

//********************************************************************************************
//读一个扇区
void ReadBlock(uint32 LBA)
//********************************************************************************************
{
    readfloppyA(LBA, BUFFER_FAT);
}



//********************************************************************************************
//读取BPB数据结构
void ReadBPB(void)
//********************************************************************************************
{
    ReadBlock(0);
    FAT_BPB* BPB = (FAT_BPB*)BUFFER_FAT;

    //缓存相关参数
    BPB_SecPerClus  =  BPB->BPB_SecPerClus;
    BPB_RsvdSecCnt  =  BPB->BPB_RsvdSecCnt;
    BPB_NumFATs     =  BPB->BPB_NumFATs;
    BPB_RootEntCnt  =  BPB->BPB_RootEntCnt;
    BPB_TotSec16    =  BPB->BPB_TotSec16;
    BPB_FATSz16     =  BPB->BPB_FATSz16;
    BPB_HiddSec     =  BPB->BPB_HiddSec;

}

//********************************************************************************************
//获取根目录开始扇区号
uint32 DirStartSec(void)
//********************************************************************************************
{
    return BPB_RsvdSecCnt + BPB_NumFATs * BPB_FATSz16;
}

//********************************************************************************************
//获取数据区开始扇区号
uint32 DataStartSec(void)
//********************************************************************************************
{
    return DirStartSec() + BPB_RootEntCnt * 32 / 512;
}






