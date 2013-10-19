#ifndef __FILE_H__
#define __FILE_H__

#include <type.h>

#define MAX_FD 10


#define NOMAL_FILE 0
#define TTY        1

typedef struct{
    uint32 isused;
    uint32 dev;
    uint32 indexno;
    uint32 offset;
    uint32 startnode;
    uint32 curnode;
    uint32 length;
} fileindex;

void initfs();
int file_open(fileindex *file,const char *path, int flags, ...);
int file_read(fileindex *file,void *buff,size_t len);
off_t file_lseek(fileindex *file,off_t offset, int whence);
int file_write(fileindex *file,const void *ptr,size_t len);  //doesn't Implement
int file_close(fileindex *file);

#endif
