
/*
 * newlib_stubs.c
 *
 *  Created on: 2 Nov 2010
 *      Author: nanoage.co.uk
 */


#include <chouryos.h>
#include <syscall.h>


void syscall(u32 eax,u32 ebx,u32 ecx,u32 edx,u32 esi,u32 edi){
    switch(eax){
    case 1:
        ecx+=(u32)PROTABLE[curpid].base;
        sys_write((int)ebx,(const void *)ecx,(size_t)edx);
        return;
    case 2:
        ecx+=(u32)PROTABLE[curpid].base;
        sys_read((int)ebx,(void *)ecx,(size_t)edx);
        return;
    case 3:
        ebx+=(u32)PROTABLE[curpid].base;
        sys_open((const char*)ebx,(int)ecx,edx);
        return;
    case 4:
        sys_close((int)ebx);
        return;
    case 5:
        sys_sbrk((int)ebx);
        return;
    case 6:
        sys_fork();
        return;
    case 7:
        sys_lseek((int)ebx,(off_t)ecx,(int)edx);
        return;
    case 8:
        ebx+=(u32)PROTABLE[curpid].base;
        ecx+=(u32)PROTABLE[curpid].base;
        sys_gettimeofday((struct timeval *)ebx,(struct timezone *)ecx);
        return;
    case 9:
        ebx+=(u32)PROTABLE[curpid].base;
        ecx+=(u32)PROTABLE[curpid].base;
        edx+=(u32)PROTABLE[curpid].base;
        sys_execve((char *)ebx,(char **)ecx,(char **)edx);
        return;
    case 10:
        sys_isatty(ebx);
        return;
    }
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
        PROTABLE[curpid].reg.eax=0;
        return 0;
    }
    if(PROTABLE[curpid].file[fd].type==TTY){
        PROTABLE[curpid].reg.eax=1;
        return 0;
    }else{
        errno = ENOTTY;
        PROTABLE[curpid].reg.eax=0;
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


