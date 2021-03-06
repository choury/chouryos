#include <common.h>
#include <file.h>
#include <process.h>
#include <errno.h>

/*
 lseek
 Set position in a file. Minimal implementation:
 */
off_t sys_lseek(int fd, off_t ptr, int dir) {
    if((fd < 0) || (fd >= MAX_FD) || (!PROTABLE[curpid].file[fd].isused)){
        errno=EBADF;
        return -1;
    }
    switch(PROTABLE[curpid].file[fd].type){
    case TTY:
    case MSG:
        errno=ESPIPE;
        return -1;
    case NOMAL_FILE:
        return file_lseek(PROTABLE[curpid].file+fd,ptr,dir);
    }
    return -1;
}
