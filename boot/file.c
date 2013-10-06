#include "boot.h"


fileindex filedesc;


void initfs(){
    reset_floppy_controller(0);         //init floppy
    FAT_Init();                         //get fat infomation struct
}


uint16 getnextnode(int node){
    ReadBlock(node*12/8/512+1);
    uint16 nextnode=*(uint16 *)(&((uint8 *)BUFFER_FAT)[(node*12/8)%512]);
    if(node&1){
        return nextnode>>4;
    }else{
        return nextnode&0x0fff;
    }
}

int cmpname(const char *namea,const char *nameb){
    int i=0;
    for(i=0;i<11;i=i+1){
        if(namea[i]!=nameb[i]){
            return 0;
        }
    }
    return 1;
}


void splitpath(const char *path,char name[]){
    int len=0,i;
    const char *tmp=path;
    while((*tmp)&&(*tmp!='.')){
        len++;
        tmp++;
    }
    if(len>8){
        for(i=0;i<6;++i){
            name[i]=path[i];
        }
        name[6]='~';
        name[7]='1';
    }else{
        for(i=0;i<len;++i){
            if(islower(path[i])){
                name[i]=toupper(path[i]);
            }else{
                name[i]=path[i];
            }
        }
        for(;i<8;++i){
            name[i]=' ';
        }
    }
    i=0;
    if(*tmp){
        path=++tmp;
        len=0;
        while(*tmp){
            len++;
            tmp++;
        }
        for(;(i<len)&&(i<3);++i){
            if(islower(path[i])){
                name[i+8]=toupper(path[i]);
            }else{
                name[i+8]=path[i];
            }
        }
    }
    for(;i<3;++i){
        name[i+8]=' ';
    }
}

int open(const char *path){
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
            if(cmpname(name, (char *)&((DIR*)&BUFFER_FAT[j * 32])->FileName))
            {
                filedesc.indexno=(i-DirStart)*16+j;
                filedesc.offset=0;
                filedesc.startnode=((DIR*)&BUFFER_FAT[j * 32])->FilePosit.Start;
                filedesc.curnode=filedesc.startnode;
                filedesc.length=((DIR*)&BUFFER_FAT[j * 32])->FilePosit.Size;
                return 0; //找到对应的目录项,返回
            }
        }
    }
    return -1; //没有找到对应的目录项,返回
}

int read(int fd,void *buff,int count){
//    printf("fd:%d,count:%x\n",fd,count);
    uint16 DataSec=DataStartSec();
    int i,readlen=0;
//    printf("File length:%d,offset:%d\n",filedesc.length,filedesc.offset);
    while(filedesc.offset%512+count>=512){
        if(filedesc.curnode>0xff8){
//            printf("read error!\n");
            return -1;
        }
        ReadBlock(DataSec+filedesc.curnode-2);
        for(i=0;(i+filedesc.offset%512<512)&&(i+filedesc.offset<filedesc.length);i=i+1,readlen=readlen+1){
            ((char*)buff)[readlen]=BUFFER_FAT[filedesc.offset%512+i];
        }
        filedesc.offset+=i;
        if(filedesc.offset%512){
//            printf("read:%d\n",readlen);
            return readlen;
        }
        filedesc.curnode=getnextnode(filedesc.curnode);
        count-=i;
    }
    if(filedesc.curnode>0xff8){
 //       printf("read error!\n");
        return -1;
    }
    ReadBlock(DataSec+filedesc.curnode-2);
    for(i=0;(i<count)&&(i+filedesc.offset<filedesc.length);i=i+1,readlen=readlen+1){
        ((char*)buff)[readlen]=BUFFER_FAT[filedesc.offset%512+i];
    }
    filedesc.offset+=i;
//    printf("read:%d\n",readlen);
    return readlen;
}
