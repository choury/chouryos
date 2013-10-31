#include <chouryos.h>
#include <syscall.h>

int sys_close(int fd) {
    if(reenter){
        putstring("close can't be called by kernel!\n");
        return -1;
    }
    if((fd<0)||(fd>=MAX_FD)||!PROTABLE[curpid].file[fd].isused){
        errno=EBADF;
        return -1;
    }
    PROTABLE[curpid].file[fd].isused=0;
    switch(PROTABLE[curpid].file[fd].type){
    case TTY:
        return 0;
    case NOMAL_FILE:
        return file_close(PROTABLE[curpid].file+fd);
    default:
        return 0;
    }
}
