#ifndef __SYSCALL_H__
#define __SYSCALL_H__


#include <type.h>
#include <file.h>
#include <graphy.h>

int syscall(uint32 eax,uint32 ebx,uint32 ecx,uint32 edx,uint32 esi,uint32 edi);

//char **environ; /* pointer to array of char * strings that define the current environment variables */

void sys_exit();
int sys_open(const char *, int, ...);
int sys_close(int fd);
int sys_execve(char *name, char **argv, char **env);
int sys_fork();
int sys_fstat(int fd, struct stat *st);
int sys_getpid();
int sys_isatty(int fd);
int sys_kill(int pid, int sig);
int sys_link(char *old, char *newname);
off_t sys_lseek(int fd, off_t offset, int whence);
int sys_open(const char *name, int flags,...);
int sys_read(int fd, void *ptr, size_t len);
void* sys_sbrk(int incr);
int sys_stat(const char *fd, struct stat *st);
int sys_gettimeofday(struct timeval *tv, struct timezone *tz);
clock_t sys_times(struct tms *buf);
int sys_unlink(char *name);
int sys_wait(int *status);
int sys_write(int fd, const void *ptr, size_t len);

#endif
