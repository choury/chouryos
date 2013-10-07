#include <syscall.h>

/*
 sbrk
 Increase program data space.
 Malloc and related functions depend on this
 */
void * sys_sbrk(int incr) {
//    errno = ENOMEM;
    return  (void *)0x300000+incr;

}
