#include <string.h>
#include <file.h>
#include <fat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <chouryos.h>
#include <hd.h>
#include <floppy.h>

supernode dev[MAX_DEV];


void initfs() {
//    reset_floppy_controller(0);         //init floppy
    resetHd(0);
    FAT_Init();                         //get fat infomation struct
}

#define upto(x,y) ((x)+(y)-((x)%(y)))       //x对y向上取整 比如 upto(12,10）= 20
#define upone(x,y) ((x)+(y)-1)/(y)          //x除以y取天棚  ┌x/y┐ 比如 upone(12,5)=3


int file_open(fileindex *file,const char *path, int flags) {
    if(Fat_open(file,path)<0) {
        errno=ENOENT;
        return -1; //没有找到对应的目录项,返回
    } else {
        file->isused=1;
        file->offset=0;
        file->curnode=file->startnode;
        file->type=NOMAL_FILE;
        file->accesstime=kernel_getnowtime();
        if(flags & O_TRUNC) {
            file->length=0;
            file->updatetime=kernel_getnowtime();
            Fat_cut(file->curnode);
        }
        if(flags & O_APPEND) {
            file_lseek(file,0,SEEK_END);
        }
        return 0; //成功返回
    }
}


int file_read(fileindex *file,void *buff,size_t len) {
    if(len+file->offset > file->length) {
        len=file->length-file->offset;
    }
    return Fat_read(file,buff,len);
}


off_t file_lseek(fileindex *file,off_t offset, int whence) {
    int tmpnode;
    switch(whence) {
    case SEEK_SET:
        if(offset < 0) {
            errno=EINVAL;
            return -1;
        }
        tmpnode=Fat_seek(file->startnode,upone(offset,file->nodebytes)-1);
        break;
    case SEEK_CUR:
        offset+=file->offset;
        if(offset<0) {
            errno=EINVAL;
            return -1;
        }
        tmpnode=Fat_seek(file->curnode,
                         upone(offset,file->nodebytes)-upone(file->offset,file->nodebytes)
                        );
        break;
    case SEEK_END:
        offset+=file->length;
        if(offset<0) {
            errno=EINVAL;
            return -1;
        }
        tmpnode=Fat_seek(file->curnode,
                         upone(offset,file->nodebytes)-upone(file->offset,file->nodebytes)
                        );
        break;
    default:
        errno=EINVAL;
        return -1;
    }
    if(tmpnode<=0) {
        errno=ENOSPC;
        return -1;
    } else {
        file->curnode=tmpnode;
    }
    file->offset=offset;
    if(file->offset > file->length) {            //文件长度被扩展
        file->length=file->offset;
    }
    return file->offset;
}

int file_write(fileindex *file,const void *ptr,size_t len) {
    uint32 tmpnode=file->curnode;
    file->updatetime=kernel_getnowtime();
    if(len+file->offset>file->length) {
        tmpnode=Fat_seek(file->curnode,
                         upone(file->length,file->nodebytes)-upone(file->offset,file->nodebytes)
                        );
        int c=Fat_expand(tmpnode,
                         upone(file->offset+len,file->nodebytes)-upone(file->length,file->nodebytes),
                         FALSE
                        );
        if(c<=0){
            file->length=upto(file->length,file->nodebytes)+(-c)*file->nodebytes;
        }else{
            file->length=file->offset+len;
        }
    }
    return Fat_write(file,ptr,len);
}


int file_close(fileindex *file) {
    return Fat_close(file);
}
