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


static uint8           BPB_SecPerClus;
static uint16          BPB_RsvdSecCnt;
static uint8           BPB_NumFATs;
static uint16          BPB_RootEntCnt;
static uint16          BPB_TotSec16;
static uint16          BPB_FATSz16;
static uint32          BPB_HiddSec;
static int             LastAccess=-1;


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
    if(LBA != LastAccess) {
        readfloppyA(LBA, BUFFER_FAT);
        LastAccess=LBA;
    }
}


//********************************************************************************************
//写一个扇区
void WriteBlock(uint32 LBA)
//********************************************************************************************
{
    writefloppyA(LBA, BUFFER_FAT);
    LastAccess=LBA;
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


//获取一个node下一个簇的序号
int getnextnode(uint32 node) {
    ReadBlock(node*12/8/512+1);
    uint16 nextnode=*(uint16 *)(&((uint8 *)BUFFER_FAT)[(node*12/8)%512]);
    if(node&1) {
        return nextnode>>4;
    } else {
        return nextnode&0x0fff;
    }
}


//获取一个node上一个簇的序号 如不存在则返回-1
int getprenode(uint32 node) {
    int n;
    for(n=2; n<0xff7; ++n) {
        ReadBlock(n*12/8/512+1);
        uint16 t=*(uint16 *)(&((uint8 *)BUFFER_FAT)[(n*12/8)%512]);
        if(n&1) {
            if(t>>4 == node) {
                return n;
            }
        } else {
            if((t&0x0fff) == node) {
                return n;
            }
        }
    }
    return -1;
}


//获取一个空白簇，如果node不为0，则会在对应簇号上填入获得的簇号
int getblanknode(uint32 node) {
    int n=2;
    for(n=2; n<0xff7; ++n) {
        ReadBlock(n*12/8/512+1);
        uint16 t=*(uint16 *)(&BUFFER_FAT[(n*12/8)%512]);
        if(n&1) {
            if(t>>4 == 0) {
                *(uint16 *)(&BUFFER_FAT[(n*12/8)%512])= (t & 0x000f)| 0xfff0;
                WriteBlock(n*12/8/512+1);
                goto next;
            }
        } else {
            if((t&0x0fff) == 0) {
                *(uint16 *)(&BUFFER_FAT[(n*12/8)%512])= (t & 0xf000)| 0xfff;
                WriteBlock(n*12/8/512+1);
                goto next;
            }
        }
    }
    return -1;
next:
    if(node != 0) {
        ReadBlock(node*12/8/512+1);
        uint16 t=*(uint16 *)(&BUFFER_FAT[(node*12/8)%512]);
        if(node&1) {
            *(uint16 *)(&BUFFER_FAT[(node*12/8)%512])=((t & 0x000f) | (n <<4));
        } else {
            *(uint16 *)(&BUFFER_FAT[(node*12/8)%512])=((t & 0xf000) | (n & 0xfff));
        }
        WriteBlock(node*12/8/512+1);
        WriteBlock(node*12/8/512+10);
    }
    return n;
}

#if 0

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

#endif
