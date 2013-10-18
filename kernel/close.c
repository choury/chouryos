#include <chouryos.h>
#include <syscall.h>

int sys_close(int fd) {
    if((fd<0)||(fd>=MAX_FD)||!PROTABLE[curpid].file[fd].isused){
        errno=EBADF;
        return -1;
    }
    PROTABLE[curpid].file[fd].isused=0;
    return 0;
}
