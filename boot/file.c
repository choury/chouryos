#include "boot.h"


fileindex filedesc;


void initfs() {
    reset_floppy_controller(0);         //init floppy
    FAT_Init();                         //get fat infomation struct
}


//获取一个node下一个簇的序号
int getnextnode(uint32 node) {
    if(node<2 || node>=0xff8) {
        return -1;
    }
    ReadBlock(node*12/8/512+1);
    if((node*12/8)%512!=511) {
        uint16 nextnode=*(uint16 *)(&BUFFER_FAT[(node*12/8)%512]);
        if(node&1) {
            return nextnode>>4;
        } else {
            return nextnode&0x0fff;
        }
    } else {
        if(node&1) {
            uint16 nextnode=BUFFER_FAT[511] >>4;
            ReadBlock(node*12/8/512+2);
            nextnode |= BUFFER_FAT[0]<<4;
            return nextnode;
        } else {
            uint16 nextnode=BUFFER_FAT[511] & 0xff;
            ReadBlock(node*12/8/512+2);
            nextnode |= (BUFFER_FAT[0] & 0xf) <<8;
            return nextnode;
        }
    }
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
            if(islower(path[i])) {
                name[i]=toupper(path[i]);
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
            if(islower(path[i])) {
                name[i+8]=toupper(path[i]);
            } else {
                name[i+8]=path[i];
            }
        }
    }
    for(; i<3; ++i) {
        name[i+8]=' ';
    }
}

int open(const char *path) {
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
            if(cmpname(name, (char *)((DIR*)BUFFER_FAT)[j].FileName))
            {
                filedesc.startnode=((DIR*)BUFFER_FAT)[j].FilePosit.Start;
                filedesc.curnode=filedesc.startnode;
                filedesc.length=((DIR*)BUFFER_FAT)[j].FilePosit.Size;
                return 0; //找到对应的目录项,返回
            }
        }
    }
    return -1; //没有找到对应的目录项,返回
}

int read(int fd,void *buff,int len) {
    uint16 DataSec=DataStartSec();
    int c,readlen=0,i;
    while(len) {
        if( (filedesc.offset%512 == 0) && (filedesc.offset) && (filedesc.offset < filedesc.length) ) {
            filedesc.curnode=getnextnode(filedesc.curnode);
        }
        ReadBlock(DataSec+filedesc.curnode-2);
        c=(512-filedesc.offset%512)<(filedesc.length-filedesc.offset)?
          (512-filedesc.offset%512):
          (filedesc.length-filedesc.offset);
        c=c<len?c:len;
        for(i=0; i<c; i=i+1) {
            ((char*)buff)[readlen+i] = BUFFER_FAT[filedesc.offset%512+i];
        }
        readlen+=c;
        filedesc.offset+=c;
        if(filedesc.offset==filedesc.length) {
            break;
        }
        len-=c;
    }
    return readlen;
}
