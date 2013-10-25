#include <type.h>
#include <file.h>

#ifndef __FAT_H__
#define __FAT_H__


enum Filetype{
    FAT12=12,FAT16=16,FAT32=32
};

typedef struct
{
    uint8         jmpBoot[3];
    uint8         OEMName[8];
    uint16        BytesPerSec; //useful
    uint8         SecPerClus;  //useful
    uint16        RsvdSecCnt;  //useful
    uint8         NumFATs;     //useful
    uint16        RootEntCnt;  //0 for fat32
    uint16        TotSec;      //use TotSec32 if 0
    uint8         Media;
    uint16        FATSecSz;    //use FATSz32 if 0
    uint16        SecPerTrk;
    uint16        NumHeads;
    uint32        HiddSec;
    uint32        TotSec32;
    uint32        FATSecSz32;
    uint16        Flags;
    uint16        Version;
    uint32        RootDirClu;   //useful
    uint16        FSInfoSec;
    uint16        BackupSec;
    uint8         Reserved[12];
    uint8         DrvNum32;
    uint8         Reserved1;
    uint8         BootSig32;
    uint32        VolID32;
    uint8         VolLab32[11];
    uint8         FileSysType32[8];
    uint8         ExecutableCode[410];
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


int Fat_open(fileindex *file,const char *path);
int Fat_read(fileindex *file,void *buff,size_t len);
int Fat_write(fileindex *file,const void *ptr,size_t len);
int Fat_expand(uint32 clus,size_t len,BOOL flag);
int Fat_cut(uint32 clus);
int Fat_seek(uint32,off_t offset);
int Fat_close(fileindex *file);

#endif
