#include <syscall.h>
#include <sys/unistd.h>
#include <chouryos.h>

/*
 lseek
 Set position in a file. Minimal implementation:
 */
off_t sys_lseek(int fd, off_t ptr, int dir) {
    if((fd < 0) || (fd >= MAX_FD) || (!PROTABLE[CURPID].file[fd].isused)){
        errno=EBADF;
        return -1;
    }
    switch(PROTABLE[CURPID].file[fd].dev){
    case TTY:
        errno=ESPIPE;
        return -1;
    case NOMAL_FILE:
        return file_lseek(PROTABLE[CURPID].file+fd,ptr,dir);
    default:
        return -1;
    }
}
