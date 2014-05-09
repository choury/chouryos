#include <fat.h>
#include <floppy.h>
#include <string.h>
#include <file.h>
#include <common.h>
#include <hd.h>
#include <malloc.h>


typedef struct
{
    uint16 m;
    uint16 ClusID;
    uint32 i;
    uint32 LBA;
} FATFileIndex;

uint8           BUFFER_FAT[1024];      //1024字节的缓冲区

static uint32          Partition_Sec;

static uint16          BPB_BytesPerSec;
static uint8           BPB_SecPerClus;
static uint16          BPB_RsvdSecCnt;
static uint8           BPB_NumFATs;
static uint32          BPB_RootEntCnt;
static uint32          BPB_TotSec;
static uint32          BPB_FATSecSz;
static uint32          BPB_RootDirClu;
static uint32          ClusBytes;
static uint32          MaxClus;
static uint32          BEOC;
static uint32          EOC;
static enum Filetype   ft;


static int             LastAccess;

static void     ReadBPB          (void);

void FAT_Init(void)
{
    uint8 buff[512];
    readHd(0,1,buff);
    Partition_Sec=((MBR*)buff)->hpt[0].sector;
    LastAccess=-1;
    ReadBPB();
}


//读一个扇区
static void ReadSector(uint32 sec)
{
    if(sec != LastAccess) {
//        readfloppyA(sec, BUFFER_FAT);
        readHd(sec+Partition_Sec,1,BUFFER_FAT);
        LastAccess=sec;
    }
}



//写一个扇区
static void WriteSector(uint32 sec)
{
    writeHd(sec+Partition_Sec,1,BUFFER_FAT);
    LastAccess=sec;
}


//读取BPB数据结构
static void ReadBPB(void)
{
    ReadSector(0);
    FAT_BPB* BPB = (FAT_BPB*)BUFFER_FAT;

    //缓存相关参数
    BPB_BytesPerSec =  BPB->BytesPerSec;
    BPB_SecPerClus  =  BPB->SecPerClus;
    BPB_RsvdSecCnt  =  BPB->RsvdSecCnt;
    BPB_NumFATs     =  BPB->NumFATs;
    BPB_RootEntCnt  =  BPB->RootEntCnt;
    if(BPB->TotSec) {
        BPB_TotSec  =  BPB->TotSec;
    } else {
        BPB_TotSec  =  BPB->TotSec32;
    }
    if(BPB->FATSecSz) {
        BPB_FATSecSz   =  BPB->FATSecSz;
    } else {
        BPB_FATSecSz   =  BPB->FATSecSz32;
    }
    if(BPB_TotSec/BPB_SecPerClus >0xFFFF) {
        ft=FAT32;
        BPB_RootDirClu=BPB->RootDirClu;
        MaxClus=0xFFFFFEF;
        BEOC=0xFFFFFF8;
        EOC=0xFFFFFFF;
    } else if(BPB_TotSec/BPB_SecPerClus >0xFFF) {
        ft=FAT16;
        MaxClus=0xFFEF;
        BEOC=0xFFF8;
        EOC=0xFFFF;
    } else {
        ft=FAT12;
        MaxClus=0xFEF;
        BEOC=0xFF8;
        EOC=0xFFF;
    }
    ClusBytes=BPB_SecPerClus*BPB_BytesPerSec;
}


//获取根目录开始扇区号
static uint32 DirStartSec(void)
{
    switch(ft) {
    case FAT12:
    case FAT16:
        return BPB_RsvdSecCnt + BPB_NumFATs * BPB_FATSecSz ;
    case FAT32:
        return BPB_RsvdSecCnt + BPB_NumFATs * BPB_FATSecSz + BPB_RootDirClu * BPB_SecPerClus;
    }
    return 0;
}


//获取数据区开始扇区号
static uint32 DataStartSec(void)
{
    switch(ft) {
    case FAT12:
    case FAT16:
        return BPB_RsvdSecCnt + BPB_NumFATs * BPB_FATSecSz + BPB_RootEntCnt * 32 / 512;
    case FAT32:
        return BPB_RsvdSecCnt + BPB_NumFATs * BPB_FATSecSz;
    }
    return 0;
}

//读一个簇
static void ReadClus(uint32 Clus,void *buff) {
    uint32 bsec=DataStartSec()+(Clus-2)*BPB_SecPerClus;
    uint32 esec=DataStartSec()+(Clus-1)*BPB_SecPerClus;
    uint32 sec;
    for(sec=bsec; sec<esec; ++sec) {
        ReadSector(sec);
        memcpy(buff,BUFFER_FAT,BPB_BytesPerSec);
        buff+=BPB_BytesPerSec;
    }
}

//写一个簇
static void WriteClus(uint32 Clus,void *buff) {
    uint32 bsec=DataStartSec()+(Clus-2)*BPB_SecPerClus;
    uint32 esec=DataStartSec()+(Clus-1)*BPB_SecPerClus;
    uint32 sec;
    for(sec=bsec; sec<esec; ++sec) {
        memcpy(BUFFER_FAT,buff,BPB_BytesPerSec);
        WriteSector(sec);
        buff+=BPB_BytesPerSec;
    }
}

//获取一个node下一个簇的序号
static uint32 getnextclus(uint32 clus) {
    if(clus<2 || clus>=BEOC) {
        return EOC;
    }
    uint32 nextclus;
    ReadSector(clus*ft/8/BPB_BytesPerSec+BPB_RsvdSecCnt);
    switch(ft) {
    case FAT12:
        if((clus*12/8)%512!=511) {
            nextclus=*(uint16 *)(&BUFFER_FAT[(clus*12/8)%512]);
            if(clus&1) {
                return nextclus>>4;
            } else {
                return nextclus&0x0fff;
            }
        } else {
            if(clus&1) {
                uint16 nextnode=BUFFER_FAT[511] >>4;
                ReadSector(clus*12/8/512+2);
                nextnode |= BUFFER_FAT[0]<<4;
                return nextnode;
            } else {
                uint16 nextnode=BUFFER_FAT[511] & 0xff;
                ReadSector(clus*12/8/512+2);
                nextnode |= (BUFFER_FAT[0] & 0xf) <<8;
                return nextnode;
            }
        }
        break;
    case FAT16:
        return ((uint16 *)BUFFER_FAT)[clus%(BPB_BytesPerSec/2)];
    case FAT32:
        return ((uint32 *)BUFFER_FAT)[clus%(BPB_BytesPerSec/4)];
    }
    return EOC;
}

//将node的簇的下一个簇号设为nextnode
static int writenextclus(uint32 clus,uint32 nextclus) {
    if(clus<2 || clus>=BEOC) {
        return -1;
    }
    switch(ft) {
    case FAT12:
        if((clus*12/8)%512!=511) {
            ReadSector(clus*12/8/512+BPB_RsvdSecCnt);
            uint16 t=*(uint16 *)(&BUFFER_FAT[(clus*12/8)%512]);
            if(clus&1) {
                *(uint16 *)(&BUFFER_FAT[(clus*12/8)%512])=((t & 0x000f) | (nextclus <<4));
            } else {
                *(uint16 *)(&BUFFER_FAT[(clus*12/8)%512])=((t & 0xf000) | (nextclus & 0xfff));
            }
            WriteSector(clus*12/8/512+BPB_RsvdSecCnt);
            WriteSector(clus*12/8/512+BPB_RsvdSecCnt+BPB_FATSecSz);
        } else {
            if(clus&1) {
                ReadSector(clus*12/8/512+BPB_RsvdSecCnt);
                BUFFER_FAT[511] = (BUFFER_FAT[511] & 0xf) | (nextclus & 0xf);
                WriteSector(clus*12/8/512+1);
                WriteSector(clus*12/8/512+10);

                ReadSector(clus*12/8/512+BPB_RsvdSecCnt+1);
                BUFFER_FAT[0] = nextclus>>4;
                WriteSector(clus*12/8/512+BPB_RsvdSecCnt+1);
                WriteSector(clus*12/8/512+BPB_RsvdSecCnt+BPB_FATSecSz+1);
            } else {
                ReadSector(clus*12/8/512+BPB_RsvdSecCnt);
                BUFFER_FAT[511] = nextclus & 0xff;
                WriteSector(clus*12/8/512+BPB_RsvdSecCnt);
                WriteSector(clus*12/8/512+BPB_RsvdSecCnt+BPB_FATSecSz);

                ReadSector(clus*12/8/512+BPB_RsvdSecCnt+1);
                BUFFER_FAT[0] = (BUFFER_FAT[0] & 0xf0) | (nextclus>>8);
                WriteSector(clus*12/8/512+BPB_RsvdSecCnt+1);
                WriteSector(clus*12/8/512+BPB_RsvdSecCnt+BPB_FATSecSz+1);
            }
        }
        break;
    case FAT16:
        ReadSector(clus*2/BPB_BytesPerSec+BPB_RsvdSecCnt);
        ((uint16 *)BUFFER_FAT)[clus%(BPB_BytesPerSec/2)]=nextclus;
        WriteSector(clus*2/BPB_BytesPerSec+BPB_RsvdSecCnt);
        WriteSector(clus*2/BPB_BytesPerSec+BPB_RsvdSecCnt+BPB_FATSecSz);
        break;
    case FAT32:
        ReadSector(clus*4/BPB_BytesPerSec+BPB_RsvdSecCnt);
        ((uint32 *)BUFFER_FAT)[clus%(BPB_BytesPerSec/4)]=nextclus;
        WriteSector(clus*4/BPB_BytesPerSec+BPB_RsvdSecCnt);
        WriteSector(clus*4/BPB_BytesPerSec+BPB_RsvdSecCnt+BPB_FATSecSz);
        break;
    }
    return 0;
}

//获取一个node上一个簇的序号 如不存在则返回EOC
static uint32 getpreclus(uint32 clus) {
    int n;
    for(n=2; n<=MaxClus; ++n) {
        if(getnextclus(n)==clus)
            return n;
    }
    return EOC;
}


//获取一个空白簇，如果node不为0，则会在对应簇号上填入获得的簇号
static uint32 getblankclus(uint32 clus) {
    int n=2;
    for(n=2; n<=MaxClus; ++n) {
        if(getnextclus(n)==0) {
            if(clus != 0) {
                writenextclus(clus,n);
            }
            return n;
        }
    }
    return EOC;

}

//释放node以后所有节点，如果flag为TRUE 则node也被释放
static int releaseclus(uint32 clus,uint8 flag) {
    if(clus<2 || clus >=BEOC) {
        return -1;
    }
    uint32 tmpclus=getnextclus(clus);
    if(flag) {
        writenextclus(clus,0);
    } else {
        writenextclus(clus,EOC);
    }
    clus=tmpclus;
    while(clus<=MaxClus) {
        tmpclus=getnextclus(tmpclus);
        writenextclus(clus,0);
        clus=tmpclus;
    };
    return 0;
}


uint8 getchecksum (const unsigned char Name[11])
{
    int i;
    uint8 sum=0;

    for (i=11; i; i--)
        sum = ((sum & 1) ? 0x80 : 0) + (sum >> 1) + *Name++;
    return sum;
}

static int cmpname(const char namea[11],const char nameb[11]) {
    int i=0;
    for(i=0; i<11; i=i+1) {
        if(namea[i]!=nameb[i]) {
            return 0;
        }
    }
    return 1;
}


static void splitpath(const char *path,char name[11]) {
    int len=0,i;
    const char *tmp=path;
    while((*tmp)&&(*tmp!='.')) {
        len++;
        tmp++;
    }
    if(len>8) {
        for(i=0; i<6; ++i) {
            name[i]=path[i];
        }
        name[6]='~';
        name[7]='1';
    } else {
        for(i=0; i<len; ++i) {
            if(islower((int)path[i])) {
                name[i]=toupper((int)path[i]);
            } else {
                name[i]=path[i];
            }
        }
        for(; i<8; ++i) {
            name[i]=' ';
        }
    }
    i=0;
    if(*tmp) {
        path=++tmp;
        len=0;
        while(*tmp) {
            len++;
            tmp++;
        }
        for(; (i<len)&&(i<3); ++i) {
            if(islower((int)path[i])) {
                name[i+8]=toupper((int)path[i]);
            } else {
                name[i+8]=path[i];
            }
        }
    }
    for(; i<3; ++i) {
        name[i+8]=' ';
    }
}

int Fat_open(filedes *file,const char *path) {
    uint32 DirSecCut, DirStart, i, j;
    DirSecCut = DataStartSec();
    DirStart = DirStartSec();
    char name[11];
    splitpath(path,name);
    switch(ft) {
    case FAT12:
    case FAT16:
        for(i = DirStart; i < DirSecCut; i++) {
            ReadSector(i);
            for(j = 0; j <BPB_BytesPerSec/sizeof(DIR); j++) {
                if(cmpname(name, (char *)((DIR*)BUFFER_FAT)[j].Name)) {
                    file->nodebytes=ClusBytes;
                    file->dirnode=i+DirStart;
                    file->indexno=j;
                    file->startnode=((DIR*)BUFFER_FAT)[j].Starth<<16 | ((DIR*)BUFFER_FAT)[j].Startl;
                    file->length=((DIR*)BUFFER_FAT)[j].Length;
                    file->createtime=kernel_mktime(((DIR*)BUFFER_FAT)[j].CreateYear+1980,
                                                   ((DIR*)BUFFER_FAT)[j].CreateMonth,
                                                   ((DIR*)BUFFER_FAT)[j].CreateDay,
                                                   ((DIR*)BUFFER_FAT)[j].CreateHour,
                                                   ((DIR*)BUFFER_FAT)[j].CreateMinute,
                                                   ((DIR*)BUFFER_FAT)[j].Create2Second*2
                                                  );

                    file->updatetime=kernel_mktime(((DIR*)BUFFER_FAT)[j].UpdateYear+1980,
                                                   ((DIR*)BUFFER_FAT)[j].UpdateMonth,
                                                   ((DIR*)BUFFER_FAT)[j].UpdateDay,
                                                   ((DIR*)BUFFER_FAT)[j].UpdateHour,
                                                   ((DIR*)BUFFER_FAT)[j].UpdateMinute,
                                                   ((DIR*)BUFFER_FAT)[j].Update2Second*2
                                                  );
                    return 0; //找到对应的目录项,返回
                }
            }
        }
        break;
    case FAT32:
        i=BPB_RootDirClu;
        DIR *buff=malloc(ClusBytes);
        while(i<=MaxClus) {
            ReadClus(i,buff);
            for(j = 0; j <ClusBytes/sizeof(DIR); j++) {
                if(cmpname(name, (char *)buff[j].Name)) {
                    file->nodebytes=ClusBytes;
                    file->dirnode=i;
                    file->indexno=j;
                    file->startnode=buff[j].Starth<<16 | buff[j].Startl;
                    file->length=buff[j].Length;
                    file->createtime=kernel_mktime(buff[j].CreateYear+1980,
                                                   buff[j].CreateMonth,
                                                   buff[j].CreateDay,
                                                   buff[j].CreateHour,
                                                   buff[j].CreateMinute,
                                                   buff[j].Create2Second*2
                                                  );

                    file->updatetime=kernel_mktime(buff[j].UpdateYear+1980,
                                                   buff[j].UpdateMonth,
                                                   buff[j].UpdateDay,
                                                   buff[j].UpdateHour,
                                                   buff[j].UpdateMinute,
                                                   buff[j].Update2Second*2
                                                  );
                    return 0; //找到对应的目录项,返回
                }
            }
            i=getnextclus(i);
        }
        free(buff);
        break;
    }
    return -1; //没有找到对应的目录项,返回
}

//len 必须小于等于 文件长度
int Fat_read(filedes *file,uint8 *ptr,size_t len) {
    int c,readlen=0;
    if(file->offset+len > file->length) {
        return -1;
    }
    void *buff=malloc(ClusBytes);
    while(len>0) {
        if( (file->offset%ClusBytes == 0) && (file->offset) ) {
            file->curnode=getnextclus(file->curnode);
        }
        
        ReadClus(file->curnode,buff);
        c=(ClusBytes-file->offset%ClusBytes)<len?
          (ClusBytes-file->offset%ClusBytes):len;
        memcpy(ptr+readlen,buff+file->offset%ClusBytes,c);
        readlen+=c;
        file->offset+=c;
        len-=c;
    }
    free(buff);
    return readlen;
}

//释放clus后面所有簇，不包括clus
int Fat_cut(uint32 clus){
    return releaseclus(clus,FALSE);
}

//从clus开始，申请len个簇 flag 为ture时，如果空间不够则释放申请的簇
//返回值为
//成功：最后一个申请到的簇号
//失败：(0-申请到的簇数量)
int Fat_expand(uint32 clus,size_t len,BOOL flag) {
    uint32 tmpclus=clus;
    int c=0;
    while(c<len) {
        tmpclus=getblankclus(tmpclus);
        if(tmpclus==EOC) {
            if(flag)
                releaseclus(clus,FALSE);
            return -c;
        }
        c++;
    }
    return tmpclus;
}

//从clus开始计算 返回offset偏移的簇号
//如果offset小于0,偏移不能在文件开始之前
//如果offset所指位置超出文件结尾，则申请新簇，如果空间不够则释放申请的簇
//返回值，失败同Fat_expand
int Fat_seek(uint32 clus,off_t offset) {
    int i=0;
    uint32 tmpclus;
    if(offset>0) {
        for(; i<offset; ++i) {
            tmpclus=getnextclus(clus);
            if(tmpclus>=BEOC) {
                return Fat_expand(clus,offset-i,TRUE);
            } else {
                clus=tmpclus;
            }
        }
    }
    if(offset<0) {
        for(; i<-offset; ++i) {
            clus=getpreclus(clus);
        }
    }
    return clus;
}

//len 必须小于等于 文件长度
int Fat_write(filedes *file,const uint8 *ptr,size_t len) {
    int c,writelen=0;
    if(file->offset+len>file->length) {
        return -1;
    }
    uint16 DataSec=DataStartSec();
    void *buff=malloc(ClusBytes);
    while(len>0) {
        if( (file->offset%ClusBytes == 0) && (file->offset)  ) {
            file->curnode=getnextclus(file->curnode);
        }
        ReadClus(DataSec+file->curnode,buff);
        c=(ClusBytes-file->offset%ClusBytes)<len?
          (ClusBytes-file->offset%ClusBytes):len;
        memcpy(buff+file->offset%ClusBytes,ptr+writelen,c);
        WriteClus(DataSec+file->curnode,buff);
        len-=c;
        file->offset+=c;
        writelen+=c;
    }
    free(buff);
    return writelen;
}

int Fat_close(filedes *file) {
    struct tm t;
    DIR *buff=malloc(ClusBytes);
    switch(ft) {
    case FAT12:
    case FAT16:
        ReadSector(file->dirnode);
        time_to_tm(file->createtime,&t);
        ((DIR*)BUFFER_FAT)[file->indexno].Create2Second=t.tm_sec/2;
        ((DIR*)BUFFER_FAT)[file->indexno].CreateMinute=t.tm_min;
        ((DIR*)BUFFER_FAT)[file->indexno].CreateHour=t.tm_hour;
        ((DIR*)BUFFER_FAT)[file->indexno].CreateDay=t.tm_mday;
        ((DIR*)BUFFER_FAT)[file->indexno].CreateMonth=t.tm_mon+1;
        ((DIR*)BUFFER_FAT)[file->indexno].CreateYear=t.tm_year-80;
        time_to_tm(file->accesstime,&t);
        ((DIR*)BUFFER_FAT)[file->indexno].AccessDay=t.tm_mday;
        ((DIR*)BUFFER_FAT)[file->indexno].AccessMonth=t.tm_mon+1;
        ((DIR*)BUFFER_FAT)[file->indexno].AccessYear=t.tm_year-80;
        time_to_tm(file->updatetime,&t);
        ((DIR*)BUFFER_FAT)[file->indexno].Update2Second=t.tm_sec/2;
        ((DIR*)BUFFER_FAT)[file->indexno].UpdateMinute=t.tm_min;
        ((DIR*)BUFFER_FAT)[file->indexno].UpdateHour=t.tm_hour;
        ((DIR*)BUFFER_FAT)[file->indexno].UpdateDay=t.tm_mday;
        ((DIR*)BUFFER_FAT)[file->indexno].UpdateMonth=t.tm_mon+1;
        ((DIR*)BUFFER_FAT)[file->indexno].UpdateYear=t.tm_year-80;

        ((DIR*)BUFFER_FAT)[file->indexno].Starth=file->startnode>>16;
        ((DIR*)BUFFER_FAT)[file->indexno].Startl=file->startnode&0xffff;
        ((DIR*)BUFFER_FAT)[file->indexno].Length=file->length;
        WriteSector(file->dirnode);
        break;
    case FAT32:
        
        ReadClus(file->dirnode,buff);
        time_to_tm(file->createtime,&t);
        buff[file->indexno].Create2Second=t.tm_sec/2;
        buff[file->indexno].CreateMinute=t.tm_min;
        buff[file->indexno].CreateHour=t.tm_hour;
        buff[file->indexno].CreateDay=t.tm_mday;
        buff[file->indexno].CreateMonth=t.tm_mon+1;
        buff[file->indexno].CreateYear=t.tm_year-80;
        time_to_tm(file->accesstime,&t);
        buff[file->indexno].AccessDay=t.tm_mday;
        buff[file->indexno].AccessMonth=t.tm_mon+1;
        buff[file->indexno].AccessYear=t.tm_year-80;
        time_to_tm(file->updatetime,&t);
        buff[file->indexno].Update2Second=t.tm_sec/2;
        buff[file->indexno].UpdateMinute=t.tm_min;
        buff[file->indexno].UpdateHour=t.tm_hour;
        buff[file->indexno].UpdateDay=t.tm_mday;
        buff[file->indexno].UpdateMonth=t.tm_mon+1;
        buff[file->indexno].UpdateYear=t.tm_year-80;

        buff[file->indexno].Starth=file->startnode>>16;
        buff[file->indexno].Startl=file->startnode&0xffff;
        buff[file->indexno].Length=file->length;
        WriteClus(file->dirnode,buff);
        break;
    }
    free(buff);
    return 0;
}

