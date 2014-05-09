#ifndef __CHOURYOS_H__
#define __CHOURYOS_H__

#include <type.h>
#include <file.h>

#define _FOPEN      (-1)    /* from sys/file.h, kernel use only */
#define _FREAD      0x0001  /* read enabled */
#define _FWRITE     0x0002  /* write enabled */
#define _FAPPEND    0x0008  /* append (writes guaranteed at the end) */
#define _FMARK      0x0010  /* internal; mark during gc() */
#define _FDEFER     0x0020  /* internal; defer for next gc pass */
#define _FASYNC     0x0040  /* signal pgrp when data ready */
#define _FSHLOCK    0x0080  /* BSD flock() shared lock present */
#define _FEXLOCK    0x0100  /* BSD flock() exclusive lock present */
#define _FCREAT     0x0200  /* open with file create */
#define _FTRUNC     0x0400  /* open with truncation */
#define _FEXCL      0x0800  /* error on open if file exists */
#define _FNBIO      0x1000  /* non blocking I/O (sys5 style) */
#define _FSYNC      0x2000  /* do all writes synchronously */
#define _FNONBLOCK  0x4000  /* non blocking I/O (POSIX style) */
#define _FNDELAY    _FNONBLOCK  /* non blocking I/O (4.2 style) */
#define _FNOCTTY    0x8000  /* don't assign a ctty on this open */

#define O_ACCMODE   (O_RDONLY|O_WRONLY|O_RDWR)

/*
 * Flag values for open(2) and fcntl(2)
 * The kernel adds 1 to the open modes to turn it into some
 * combination of FREAD and FWRITE.
 */
#define O_RDONLY    0       /* +1 == FREAD */
#define O_WRONLY    1       /* +1 == FWRITE */
#define O_RDWR      2       /* +1 == FREAD|FWRITE */
#define O_APPEND    _FAPPEND
#define O_CREAT     _FCREAT
#define O_TRUNC     _FTRUNC
#define O_EXCL      _FEXCL
#define O_SYNC      _FSYNC
/*  O_NDELAY    _FNDELAY    set in include/fcntl.h */
/*  O_NDELAY    _FNBIO      set in include/fcntl.h */
#define O_NONBLOCK  _FNONBLOCK
#define O_NOCTTY    _FNOCTTY


# define    SEEK_SET    0
# define    SEEK_CUR    1
# define    SEEK_END    2

#define STDIN_FILENO    0       /* standard input file descriptor */
#define STDOUT_FILENO   1       /* standard output file descriptor */
#define STDERR_FILENO   2       /* standard error file descriptor */


void exit();
int open(const char *, int, ...);
int close(int fd);
int execve(char *name, char **argv, char **env);
int fork();
int fstat(int fd, struct stat *st);
int getpid();
int isatty(int fd);
int kill(int pid, int sig);
int link(char *old, char *newname);
off_t lseek(int fd, off_t offset, int whence);
int open(const char *name, int flags,...);
int read(int fd, void *ptr, size_t len);
void* sbrk(int incr);
int stat(const char *fd, struct stat *st);
int gettimeofday(struct timeval *tv, struct timezone *tz);
clock_t sys_times(struct tms *buf);
int unlink(char *name);
int wait(int *status);
int write(int fd, const void *ptr, size_t len);

#endif