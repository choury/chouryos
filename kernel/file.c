#include <stdio.h>
#include <file.h>
#include <ctype.h>
#include <floppy.h>
#include <fat.h>
#include <errno.h>



void initfs(){
    FAT_Init();                         //get fat infomation struct
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
            if(islower((int)path[i])){
                name[i]=toupper((int)path[i]);
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
            if(islower((int)path[i])){
                name[i+8]=toupper((int)path[i]);
            }else{
                name[i+8]=path[i];
            }
        }
    }
    for(;i<3;++i){
        name[i+8]=' ';
    }
}

int file_open(const char *path, int flags, ...){
    (void)flags;
    uint16 DirSecCut, DirStart, i, j;
    int cur_index=-1;
    for(i=0;i<MAX_FD;i++){
        if(!PROTABLE[CURPID].file[i].isused){
            cur_index=i;
            break;
        }
    }
    if(cur_index<0){
        errno=EMFILE;
        return -1;
    }
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
                PROTABLE[CURPID].file[cur_index].isused=1;
                PROTABLE[CURPID].file[cur_index].indexno=(i-DirStart)*16+j;
                PROTABLE[CURPID].file[cur_index].offset=0;
                PROTABLE[CURPID].file[cur_index].startnode=((DIR*)&BUFFER_FAT[j * 32])->FilePosit.Start;
                PROTABLE[CURPID].file[cur_index].curnode=PROTABLE[CURPID].file[cur_index].startnode;
                PROTABLE[CURPID].file[cur_index].length=((DIR*)&BUFFER_FAT[j * 32])->FilePosit.Size;
                return cur_index; //找到对应的目录项,返回
            }
        }
    }
    errno=ENOENT;
    return -1; //没有找到对应的目录项,返回
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


int file_read(int fd,void *buff,size_t len){
//    printf("fd:%d,count:%x\n",fd,count);
    uint16 DataSec=DataStartSec();
    int i,readlen=0;
    if((fd<0)||(fd>=MAX_FD)||!PROTABLE[CURPID].file[fd].isused){
        return -1;
    }
//    printf("File length:%d,offset:%d\n",filedesc[fd].length,filedesc[fd].offset);
    while(PROTABLE[CURPID].file[fd].offset%512+len>=512){
        if(PROTABLE[CURPID].file[fd].curnode>0xff8){
            return -1;
        }
        ReadBlock(DataSec+PROTABLE[CURPID].file[fd].curnode-2);
        for(i=0;(i+PROTABLE[CURPID].file[fd].offset%512<512)&&(i+PROTABLE[CURPID].file[fd].offset
            <PROTABLE[CURPID].file[fd].length);i=i+1,readlen=readlen+1){
            ((char*)buff)[readlen]=BUFFER_FAT[PROTABLE[CURPID].file[fd].offset%512+i];
        }
        PROTABLE[CURPID].file[fd].offset+=i;
        if(PROTABLE[CURPID].file[fd].offset%512){
            return readlen;
        }
        PROTABLE[CURPID].file[fd].curnode=getnextnode(PROTABLE[CURPID].file[fd].curnode);
        len-=i;
    }
    if(PROTABLE[CURPID].file[fd].curnode>0xff8){
        return -1;
    }
    ReadBlock(DataSec+PROTABLE[CURPID].file[fd].curnode-2);
    for(i=0;(i<len)&&(i+PROTABLE[CURPID].file[fd].offset
        <PROTABLE[CURPID].file[fd].length);i=i+1,readlen=readlen+1){
        ((char*)buff)[readlen]=BUFFER_FAT[PROTABLE[CURPID].file[fd].offset%512+i];
    }
    PROTABLE[CURPID].file[fd].offset+=i;
    return readlen;
}

int file_write(int fd,const void *ptr,size_t len){
    return 0;
}
