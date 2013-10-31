#include <syscall.h>
#include <chouryos.h>

/*
 sbrk
 Increase program data space.
 Malloc and related functions depend on this
 */
void * sys_sbrk(int incr) {
//    errno = ENOMEM;
    if(reenter){
        putstring("sbrk can't be called by kernel!\n");
    }
    return  PROTABLE[curpid].heap+incr;
}
