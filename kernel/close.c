#include <chouryos.h>
#include <syscall.h>

int sys_close(int fd) {
    if((fd<0)||(fd>=MAX_FD)||!PROTABLE[curpid].file[fd].isused){
        errno=EBADF;
        return -1;
    }
    PROTABLE[curpid].file[fd].isused=0;
    switch(PROTABLE[curpid].file[fd].dev){
    case TTY:
        return 0;
    case NOMAL_FILE:
        return file_close(PROTABLE[curpid].file);
    default:
        return 0;
    }
}
