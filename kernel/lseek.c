#include <syscall.h>
#include <sys/unistd.h>
#include <chouryos.h>

/*
 lseek
 Set position in a file. Minimal implementation:
 */
off_t sys_lseek(int fd, off_t ptr, int dir) {
    if((fd < 0) || (fd >= MAX_FD) || (!PROTABLE[curpid].file[fd].isused)){
        errno=EBADF;
        PROTABLE[curpid].reg.eax=-1;
        return 0;
    }
    switch(PROTABLE[curpid].file[fd].type){
    case TTY:
        errno=ESPIPE;
        PROTABLE[curpid].reg.eax=-1;
        return 0;
    case NOMAL_FILE:
        PROTABLE[curpid].reg.eax=file_lseek(PROTABLE[curpid].file+fd,ptr,dir);
        return 0;
    }
    PROTABLE[curpid].reg.eax=-1;
    return 0;
}
