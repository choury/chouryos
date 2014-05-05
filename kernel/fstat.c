#include <sys/stat.h>
#include <chouryos.h>
#include <process.h>

/*
 fstat
 Status of an open file. For consistency with other minimal implementations in these examples,
 all files are regarded as character special devices.
 The `sys/stat.h' header file required is distributed in the `include' subdirectory for this C library.
 */
int sys_fstat(int fd, struct stat *st) {
    st->st_size=PROTABLE[curpid].file[fd].length;
    st->st_ino=PROTABLE[curpid].file[fd].startnode;
    st->st_mode = S_IFREG | 0444;
    st->st_uid=0;
    st->st_gid=0;
    return 0;
}
