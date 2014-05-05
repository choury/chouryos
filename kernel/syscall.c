
/*
 * newlib_stubs.c
 *
 *  Created on: 2 Nov 2010
 *      Author: nanoage.co.uk
 */


#include <chouryos.h>
#include <syscall.h>
#include <file.h>
#include <process.h>

int syscall(uint32 eax,uint32 ebx,uint32 ecx,uint32 edx,uint32 esi,uint32 edi){
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
    case 6:
        return sys_fork();
    case 7:
        return sys_lseek((int)ebx,(off_t)ecx,(int)edx);
    case 8:
        return sys_gettimeofday((struct timeval *)ebx,(struct timezone *)ecx);
    case 9:
        return sys_execve((char *)ebx,(char **)ecx,(char **)edx);
    case 10:
        return sys_isatty(ebx);
    }
    return -1;
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
    return curpid;
}

/*
 isatty
 Query whether output stream is a terminal. For consistency with the other minimal implementations,
 */
int sys_isatty(int fd) {
    if((fd < 0) || (fd >= MAX_FD) || (!PROTABLE[curpid].file[fd].isused)){
        errno=EBADF;
        return 0;
    }
    if(PROTABLE[curpid].file[fd].type==TTY){
        return 1;
    }else{
        errno = ENOTTY;
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
 stat
 Status of a file (by name). Minimal implementation:
 int    _EXFUN(stat,( const char *__path, struct stat *__sbuf ));
 */

int sys_stat(const char *filepath, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
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


