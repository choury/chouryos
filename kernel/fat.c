#include "fat.h"
#include "chouryos.h"
#include "floppy.h"

typedef struct
{
    uint16 m;
    uint16 ClusID;
    uint32 i;
    uint32 LBA;
} FATFileIndex;

uint8           BUFFER_FAT[512];      //512字节的缓冲区

static FATFileIndex    FileIndex;        //当前打开的文件
static uint8           BPB_SecPerClus;
static uint16          BPB_RsvdSecCnt;
static uint8           BPB_NumFATs;
static uint16          BPB_RootEntCnt;
static uint16          BPB_TotSec16;
static uint16          BPB_FATSz16;
static uint32          BPB_HiddSec;


static uint8    IsEqual          (void* A, void* B, uint8 Size);
static void     ReadBPB          (void);
static uint32   ClusConvLBA      (uint16 ClusID);
static uint16   ReadFAT          (uint16 Index);
static uint8    GetFileID        (uint8 Name[11], DIR* ID);

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
//写一个扇区
void WriteBlock(uint32 LBA)
//********************************************************************************************
{
    writefloppyA(LBA, BUFFER_FAT);
}


//********************************************************************************************
static uint8 IsEqual(void* A, void* B, uint8 Size)
//********************************************************************************************
{
    uint8 i, *a = A, *b = B;
    for(i = 0; i < Size; i++)
        if(a[i] != b[i])
            return 0;
    return 1;
}

//********************************************************************************************
//读取BPB数据结构
void ReadBPB(void)
//********************************************************************************************
{
    FAT_BPB* BPB = (FAT_BPB*)BUFFER_FAT;
    ReadBlock(0);

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



//********************************************************************************************
//获取一个簇的开始扇区
static uint32 ClusConvLBA(uint16 ClusID)
//********************************************************************************************
{
    return DataStartSec() + BPB_SecPerClus * (ClusID - 2);
}

//********************************************************************************************
//读取文件分配表的指定项
static uint16 ReadFAT(uint16 Index)
//********************************************************************************************
{
    uint16 *RAM = (uint16*)BUFFER_FAT;
    ReadBlock(BPB_RsvdSecCnt + Index / 256);
    return RAM[Index % 256];
}

//********************************************************************************************
//获得和文件名对应的目录项
static uint8 GetFileID(uint8 Name[11], DIR* ID)
//********************************************************************************************
{
    uint16 i, m;
    for(i = DirStartSec(); i < DataStartSec(); i++)
    {
        ReadBlock(i);
        for(m = 0; m <16; m++)
        {
            if(IsEqual(Name, &((DIR*)&BUFFER_FAT[m * 32])->FileName, 11))
            {
                *ID = *((DIR*)&BUFFER_FAT[m * 32]);
                return 1; //找到对应的目录项,返回1
            }
        }
    }
    return 0; //没有找到对应的目录项,返回0
}

//********************************************************************************************
//打开指定文件
void FAT_FileOpen(uint8 Name[11], uint32 Start)
//********************************************************************************************
{
    uint16 BytePerClus, ClusNum;
    DIR FileDir;
    BytePerClus = BPB_SecPerClus * 512; // 每簇的字节数
    GetFileID(Name, &FileDir);

    //计算开始位置所在簇的簇号
    ClusNum = Start / BytePerClus;
    FileIndex.ClusID = FileDir.FilePosit.Start;
    for(FileIndex.i = 0; FileIndex.i < ClusNum; FileIndex.i++)
        FileIndex.ClusID = ReadFAT(FileIndex.ClusID);


    FileIndex.i = (Start % BytePerClus) / 512; //开始位置所在扇区簇内偏移
    FileIndex.m = (Start % BytePerClus) % 512; //开始位置扇区内偏移

    FileIndex.LBA = ClusConvLBA(FileIndex.ClusID) + FileIndex.i; //开始位置所在的扇区号
    ReadBlock(FileIndex.LBA); //预读取一个扇区的内容
}

//********************************************************************************************
//读取文件的数据
void FAT_FileRead(uint32 Length, void* Data)
//********************************************************************************************
{
    uint8 *data = Data;

    goto FAT_FileRead_Start;

    while(1)
    {
        FileIndex.ClusID = ReadFAT(FileIndex.ClusID); //下一簇簇号
        FileIndex.LBA = ClusConvLBA(FileIndex.ClusID);
        FileIndex.i = 0;
        while(FileIndex.i < BPB_SecPerClus)
        {
            ReadBlock(FileIndex.LBA);
            FileIndex.m = 0;
FAT_FileRead_Start:
            while(FileIndex.m < 512)
            {
                *data++ = BUFFER_FAT[FileIndex.m];
                FileIndex.m++;
                //如果读取完成就退出
                if(--Length == 0)
                    return;
            }
            FileIndex.LBA++;
            FileIndex.i++;
        }
    }
}

//********************************************************************************************
//写文件的数据
void FAT_FileWrite(uint32 Length, void* Data)
//********************************************************************************************
{
    uint8 *data = Data;

    goto FAT_FileWrite_Start;

    while(1)
    {
        FileIndex.ClusID = ReadFAT(FileIndex.ClusID); //下一簇簇号
        FileIndex.LBA = ClusConvLBA(FileIndex.ClusID);
        FileIndex.i = 0;
        while(FileIndex.i < BPB_SecPerClus)
        {
            ReadBlock(FileIndex.LBA);
            FileIndex.m = 0;
FAT_FileWrite_Start:
            while(FileIndex.m < 512)
            {
                BUFFER_FAT[FileIndex.m] = *data++;
                FileIndex.m++;
                //如果读取完成就退出
                if(--Length == 0)
                {
                    WriteBlock(FileIndex.LBA); //回写扇区
                    return;
                }
            }
            WriteBlock(FileIndex.LBA++); //回写扇区,指针下移
            FileIndex.i++;
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------

