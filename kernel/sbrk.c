#include <syscall.h>
#include <chouryos.h>
#include <process.h>

/*
 sbrk
 Increase program data space.
 Malloc and related functions depend on this
 */
void * sys_sbrk(int incr) {
//    errno = ENOMEM;
    return PROTABLE[curpid].heap+incr;
}
