#include <common.h>
#include <file.h>
#include <process.h>
#include <errno.h>




int sys_open(const char *path, int flags, ...){
    int i;
    int fd=-1;
    for(i=0;i<MAX_FD;i++){
        if(!PROTABLE[curpid].file[i].isused){
            fd=i;
            break;
        }
    }
    if(fd<0){
        errno=EMFILE;
        return -1;
    }
    if(file_open(PROTABLE[curpid].file+fd,path,flags)<0){
        return -1;
    }else{
        return fd;
    }
}
