#include <chouryos.h>
#include <syscall.h>

int sys_close(int fd) {
    if((fd<0)||(fd>=MAX_FD)||!PROTABLE[curpid].file[fd].isused){
        errno=EBADF;
        PROTABLE[curpid].reg.eax=-1;
        return 0;
    }
    PROTABLE[curpid].file[fd].isused=0;
    switch(PROTABLE[curpid].file[fd].type){
    case TTY:
        PROTABLE[curpid].reg.eax=0;
        return 0;
    case NOMAL_FILE:
        PROTABLE[curpid].reg.eax=file_close(PROTABLE[curpid].file+fd);
        return 0;
    }
    PROTABLE[curpid].reg.eax=-1;
    return 0;
}
