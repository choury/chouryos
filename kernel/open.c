#include <syscall.h>
#include <chouryos.h>
#include <file.h>







int sys_open(const char *path, int flags, ...){
    int i;
    int fd;
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
