#include <common.h>
#include <file.h>
#include <process.h>
#include <errno.h>

int sys_close(int fd) {
    if((fd<0)||(fd>=MAX_FD)||!PROTABLE[curpid].file[fd].isused){
        errno=EBADF;
        return -1;
    }
    PROTABLE[curpid].file[fd].isused=0;
    switch(PROTABLE[curpid].file[fd].type){
    case TTY:
    case MSG:
        return 0;
    case NOMAL_FILE:
        return file_close(PROTABLE[curpid].file+fd);
    }
    return -1;
}
