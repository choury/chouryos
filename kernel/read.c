#include <syscall.h>
#include <keyboad.h>
#include <file.h>



/*
 read
 Read a character to a file. `libc' subroutines will use this system routine for input from all files, including stdin
 Returns -1 on error or blocks until the number of characters have been read.
 */



int sys_read(int fd, void *ptr, size_t len) {
    size_t count=0;
    switch(fd){
    case STDIN_FILENO:
    case STDOUT_FILENO:
    case STDERR_FILENO:
        while(count<len){
            ((char *)ptr)[count++]=getone();
        }
        return len;
    default:
        return file_read(fd,ptr,len);
    }
    return 0;
}
