#include <stdio.h>
#include <string.h>
#include <file.h>
#include <ctype.h>
#include <fat.h>
#include <errno.h>
#include <fcntl.h>


void initfs() {
    FAT_Init();                         //get fat infomation struct
}


int cmpname(const char *namea,const char *nameb) {
    int i=0;
    for(i=0; i<11; i=i+1) {
        if(namea[i]!=nameb[i]) {
            return 0;
        }
    }
    return 1;
}


void splitpath(const char *path,char name[]) {
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


int file_open(fileindex *file,const char *path, int flags) {
    uint16 DirSecCut, DirStart, i, j;
    DirSecCut = DataStartSec();
    DirStart = DirStartSec();
    char name[11];
    splitpath(path,name);
    for(i = DirStart; i < DirSecCut; i++)
    {
        ReadBlock(i);
        for(j = 0; j <16; j++)
        {
            if(cmpname(name, (char *)((DIR*)BUFFER_FAT)[j].Name))
            {
                file->isused=1;
                file->indexno=(i-DirStart)*16+j;
                file->offset=0;
                file->startnode=((DIR*)BUFFER_FAT)[j].Starth<<16 | ((DIR*)BUFFER_FAT)[j].Startl;
                file->curnode=file->startnode;
                file->length=((DIR*)BUFFER_FAT)[j].Length;
                file->dev=NOMAL_FILE;
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

                file->accesstime=kernel_getnowtime();
                if(flags & O_TRUNC){
                    file->length=0;
                    file->updatetime=kernel_getnowtime();
                    releasenode(file->curnode,FALSE);
                }
                return 0; //找到对应的目录项,返回
            }
        }
    }
    errno=ENOENT;
    return -1; //没有找到对应的目录项,返回
}


int file_read(fileindex *file,void *buff,size_t len) {
    uint16 DataSec=DataStartSec();
    int c,readlen=0;
    while(len) {
        if( (file->offset%512 == 0) && (file->offset) && (file->offset < file->length) ) {
            file->curnode=getnextnode(file->curnode);
        }
        ReadBlock(DataSec+file->curnode-2);
        c=(512-file->offset%512)<(file->length-file->offset)?
          (512-file->offset%512):
          (file->length-file->offset);
        c=c<len?c:len;
        memcpy(buff+readlen,BUFFER_FAT+file->offset%512,c);
        readlen+=c;
        file->offset+=c;
        if(file->offset==file->length) {
            errno=0;
            break;
        }
        len-=c;
    }
    return readlen;
}


off_t file_lseek(fileindex *file,off_t offset, int whence) {
    int startnode,tmpnode;
    int startoffset,tmpoffset;
    switch(whence) {
    case SEEK_SET:
        startnode=file->startnode;
        startoffset=0;
        break;
    case SEEK_CUR:
        startnode=file->curnode;
        startoffset=file->offset;
        break;
    case SEEK_END:
        startnode=file->curnode;
        startoffset=file->length;
        while(1) {
            tmpnode=getnextnode(startnode);
            if(tmpnode >= 0xff8) {
                break;
            } else {
                startnode = tmpnode;
            }
        }
        break;
    default:
        errno=EINVAL;
        return -1;
    }
    tmpoffset=offset;
    tmpnode=0;
    if(offset>0) {
        if(startoffset){
            tmpoffset-=(512-(startoffset-1)%512-1);
            startoffset+=(512-(startoffset-1)%512-1);
        }else{
            tmpoffset-=512;
            startoffset+=512;
        }
        while(1) {
            if(tmpoffset<=0){
                break;
            }
            if(tmpnode<0xff8){
                tmpnode=getnextnode(startnode);
                if(tmpnode<0xff8) {
                    startnode=tmpnode;
                }
            }
            if(tmpnode>=0xff8) {
                startnode=getblanknode(startnode);
                if(startnode<0) {                     //TODO There is a bug,it doesn't release the node it applied.
                    errno=ENOSPC;
                    return -1;
                }
            }
            tmpoffset-=512;
            startoffset+=512;
        }
    } else if(offset<0) {
        if(offset+file->offset < 0) {
            errno=EINVAL;
            return -1;
        }
        tmpoffset+=(startoffset-1)%512+1;
        startoffset-=(startoffset-1)%512+1;
        while(1) {
            if(tmpoffset>=0){
                break;
            }
            startnode=getprenode(startnode);
            tmpoffset+=512;
            startoffset-=512;
        }
    }
    file->curnode=startnode;
    file->offset=startoffset+tmpoffset;
    if(file->offset > file->length) {            //文件长度被扩展
        file->length=file->offset;
    }
    return file->offset;
}

int file_write(fileindex *file,const void *ptr,size_t len) {
    uint16 DataSec=DataStartSec();
    int c,writelen=0;
    file->updatetime=kernel_getnowtime();
    while(len){
        if( (file->offset%512 == 0) && (file->offset) && (file->offset < file->length) ) {
            file->curnode=getnextnode(file->curnode);
        }
        if( (file->offset%512 == 0) && (file->offset >= file->length) ){
            int tmpnode=getblanknode(file->curnode);
            if(tmpnode>0){
                file->curnode=tmpnode;
            }else{
                errno=ENOSPC;
                break;
            }
        }
        ReadBlock(DataSec+file->curnode-2);
        c=(512-file->offset%512)<len?
          (512-file->offset%512):len;
        memcpy(BUFFER_FAT+file->offset%512,ptr+writelen,c);
        WriteBlock(DataSec+file->curnode-2);
        len-=c;
        file->offset+=c;
        writelen+=c;
    }
    if(file->offset > file->length){
        file->length=file->offset;
    }
    return writelen;
}


int file_close(fileindex *file){
    struct tm* t;
    ReadBlock(DirStartSec()+file->indexno/16);
    t=gmtime(&file->createtime);
    ((DIR*)BUFFER_FAT)[file->indexno%16].Create2Second=t->tm_sec/2;
    ((DIR*)BUFFER_FAT)[file->indexno%16].CreateMinute=t->tm_min;
    ((DIR*)BUFFER_FAT)[file->indexno%16].CreateHour=t->tm_hour;
    ((DIR*)BUFFER_FAT)[file->indexno%16].CreateDay=t->tm_mday;
    ((DIR*)BUFFER_FAT)[file->indexno%16].CreateMonth=t->tm_mon+1;
    ((DIR*)BUFFER_FAT)[file->indexno%16].CreateYear=t->tm_year-80;
    t=gmtime(&file->accesstime);
    ((DIR*)BUFFER_FAT)[file->indexno%16].AccessDay=t->tm_mday;
    ((DIR*)BUFFER_FAT)[file->indexno%16].AccessMonth=t->tm_mon+1;
    ((DIR*)BUFFER_FAT)[file->indexno%16].AccessYear=t->tm_year-80;
    t=gmtime(&file->updatetime);
    ((DIR*)BUFFER_FAT)[file->indexno%16].Update2Second=t->tm_sec/2;
    ((DIR*)BUFFER_FAT)[file->indexno%16].UpdateMinute=t->tm_min;
    ((DIR*)BUFFER_FAT)[file->indexno%16].UpdateHour=t->tm_hour;
    ((DIR*)BUFFER_FAT)[file->indexno%16].UpdateDay=t->tm_mday;
    ((DIR*)BUFFER_FAT)[file->indexno%16].UpdateMonth=t->tm_mon+1;
    ((DIR*)BUFFER_FAT)[file->indexno%16].UpdateYear=t->tm_year-80;

    ((DIR*)BUFFER_FAT)[file->indexno%16].Starth=file->startnode>>16;
    ((DIR*)BUFFER_FAT)[file->indexno%16].Startl=file->startnode&0xffff;
    ((DIR*)BUFFER_FAT)[file->indexno%16].Length=file->length;

    WriteBlock(DirStartSec()+file->indexno/16);
    return 0;
}
