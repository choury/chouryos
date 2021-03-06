
/*
 * syscall.c
 *
 *  Created on: 28 Nov 2013
 *      Author: choury
 */


#include <common.h>
#include <syscall.h>
#include <process.h>
#include <errno.h>
#include <fcntl.h>

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
    case 11:
        return sys_message(ebx,ecx);
    case 12:
        return sys_loadmod((const char*)ebx);
    case 17:
        return sys_kill(ebx, ecx);
    case 18:
        return sys_wait((int *)ebx);
    case 19:
        sys_exit(ebx);
        break;
    case 20:
        return sys_getpid();
    case 21:
        return sys_getppid();
    }
    errno=EINVAL;
    return -1;
}



/*
 getpid
 Process-ID; this is sometimes used to generate strings unlikely to conflict with other processes. Minimal implementation, for a system without processes:
 */

pid_t sys_getpid() {
    return curpid;
}


pid_t sys_getppid(){
    return PROTABLE[curpid].ppid;
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




