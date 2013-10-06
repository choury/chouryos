#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>
#include <type.h>

int syscall(u32 eax,u32 ebx,u32 ecx,u32 edx,u32 esi,u32 edi);

//char **environ; /* pointer to array of char * strings that define the current environment variables */

void sys_exit();
int sys_open(const char *, int, ...);
int sys_close(int file);
int sys_execve(char *name, char **argv, char **env);
int sys_fork();
int sys_fstat(int file, struct stat *st);
int sys_getpid();
int sys_isatty(int file);
int sys_kill(int pid, int sig);
int sys_link(char *old, char *newname);
off_t sys_lseek(int file, off_t ptr, int dir);
int sys_open(const char *name, int flags,...);
int sys_read(int file, void *ptr, size_t len);
void* sys_sbrk(int incr);
int sys_stat(const char *file, struct stat *st);
clock_t sys_times(struct tms *buf);
int sys_unlink(char *name);
int sys_wait(int *status);
int sys_write(int file, const void *ptr, size_t len);

#endif
