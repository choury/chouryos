#include <syscall.h>
#include <chouryos.h>

/*
 execve
 Transfer control to a new process.
 */
int sys_execve(char *name, char **argv, char **env) {
    //TODO .....
  // fuck,今天要给周威当苦力了吗
    errno = ENOMEM;
    return -1;
}
