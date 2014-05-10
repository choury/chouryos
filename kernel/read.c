#include <file.h>
#include <common.h>
#include <process.h>
#include <errno.h>
#include <msg.h>
#include <tty.h>

/*
 read
 Read a character to a file. `libc' subroutines will use this system routine for input from all files, including stdin
 Returns -1 on error or blocks until the number of characters have been read.
 */



int sys_read(int fd, void *ptr, size_t len) {
    if((fd < 0) || (fd >= MAX_FD) || (!PROTABLE[curpid].file[fd].isused)){
        errno=EBADF;
        return -1;
    }
    switch(PROTABLE[curpid].file[fd].type){
    case TTY:
        return tty_read(ptr,len);
    case MSG:
        return msg_read(PROTABLE[curpid].file[fd].taget.dest,ptr,len);
    case NOMAL_FILE:
        return file_read(PROTABLE[curpid].file+fd,ptr,len);
    }
    return -1;
}
