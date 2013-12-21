#include <syscall.h>
#include <chouryos.h>

/*
 sbrk
 Increase program data space.
 Malloc and related functions depend on this
 */
void * sys_sbrk(int incr) {
//    errno = ENOMEM;
    PROTABLE[curpid].reg.eax=(u32)PROTABLE[curpid].heap+incr;
    return  0;
}
