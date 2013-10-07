
/*
 * newlib_stubs.c
 *
 *  Created on: 2 Nov 2010
 *      Author: nanoage.co.uk
 */

#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <chouryos.h>
#include <syscall.h>


int syscall(u32 eax,u32 ebx,u32 ecx,u32 edx,u32 esi,u32 edi){
    switch(eax){
    case 1:
        return sys_write((int)ebx,(const void *)ecx,(size_t)edx);
    case 2:
        return sys_read((int)ebx,(void *)ecx,(size_t)edx);
    case 3:
        return sys_open((const char*)ebx,(int)ecx,edx);
    case 4:
        return sys_close((int)ebx);
    case 5:
        return (int)sys_sbrk((int)ebx);
    }
    return 0;
}


/*
 environ
 A pointer to a list of environment variables and their values.
 For a minimal environment, this empty list is adequate:
 */
//char *__env[1] = { 0 };
//char **environ = __env;


void sys_exit(int status) {
    while (1) {
        ;
    }
}


/*
 getpid
 Process-ID; this is sometimes used to generate strings unlikely to conflict with other processes. Minimal implementation, for a system without processes:
 */

int sys_getpid() {
    return CURPID;
}

/*
 isatty
 Query whether output stream is a terminal. For consistency with the other minimal implementations,
 */
int sys_isatty(int file) {
    switch (file) {
    case STDOUT_FILENO:
    case STDERR_FILENO:
    case STDIN_FILENO:
        return 1;
    default:
        //errno = ENOTTY;
        errno = EBADF;
        return 0;
    }
}


/*
 kill
 Send a signal. Minimal implementation:
 */
int sys_kill(int pid, int sig) {
    errno = EINVAL;
    return (-1);
}

/*
 link
 Establish a new name for an existing file. Minimal implementation:
 */

int sys_link(char *old, char *newname) {
    errno = EMLINK;
    return -1;
}

/*
 lseek
 Set position in a file. Minimal implementation:
 */
off_t sys_lseek(int file, off_t ptr, int dir) {
    return 0;
}



/*
 stat
 Status of a file (by name). Minimal implementation:
 int    _EXFUN(stat,( const char *__path, struct stat *__sbuf ));
 */

int sys_stat(const char *filepath, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

/*
 times
 Timing information for current process. Minimal implementation:
 */

clock_t sys_times(struct tms *buf) {
    return -1;
}

/*
 unlink
 Remove a file's directory entry. Minimal implementation:
 */
int sys_unlink(char *name) {
    errno = ENOENT;
    return -1;
}

/*
 wait
 Wait for a child process. Minimal implementation:
 */
int sys_wait(int *status) {
    errno = ECHILD;
    return -1;
}


