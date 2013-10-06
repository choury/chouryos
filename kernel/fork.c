#include <syscall.h>
#include <chouryos.h>

/*
 fork
 Create a new process
 */

int sys_fork() {
    int i;
    for(i=0;i<MAX_PROCESS;++i){
        if(PROTABLE[CURPID].isused==0)break;
    }
    if(i==MAX_PROCESS){
        errno = EAGAIN;
    }
    return -1;
}
