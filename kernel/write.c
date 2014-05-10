#include <common.h>
#include <file.h>
#include <process.h>
#include <errno.h>
#include <msg.h>
#include <tty.h>


/*
 write
 Write a character to a file. `libc' subroutines will use this system routine for output to all files, including stdout
 Returns -1 on error or number of bytes sent
 */

int sys_write(int fd,const void *ptr,size_t len) {
    if((fd < 0) || (fd >= MAX_FD) || (!PROTABLE[curpid].file[fd].isused)) {
        errno=EBADF;
        return -1;
    }
    switch(PROTABLE[curpid].file[fd].type) {
    case TTY:
        return tty_write(ptr,len);
    case MSG:
        return msg_write(PROTABLE[curpid].file[fd].taget.dest,ptr,len);
    case NOMAL_FILE:
        return file_write(PROTABLE[curpid].file+fd,ptr,len);
    }
    return -1;
}

