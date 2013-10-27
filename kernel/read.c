#include <syscall.h>
#include <keyboad.h>
#include <file.h>
#include <chouryos.h>



/*
 read
 Read a character to a file. `libc' subroutines will use this system routine for input from all files, including stdin
 Returns -1 on error or blocks until the number of characters have been read.
 */



int sys_read(int fd, void *ptr, size_t len) {
    size_t count=0;
    if((fd < 0) || (fd >= MAX_FD) || (!PROTABLE[curpid].file[fd].isused)){
        errno=EBADF;
        return -1;
    }
    switch(PROTABLE[curpid].file[fd].type){
    case TTY:
        while(count<len){
            ((char *)ptr)[count++]=getone();
        }
        return len;
    case NOMAL_FILE:
        return file_read(PROTABLE[curpid].file+fd,ptr,len);
    default:
        return -1;
    }
}
