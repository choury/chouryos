#include <syscall.h>
#include <chouryos.h>

/*
 execve
 Transfer control to a new process.
 */
int sys_execve(char *name, char **argv, char **env) {
    //TODO .....
    errno = ENOMEM;
    return -1;
}
