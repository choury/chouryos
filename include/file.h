#ifndef __FILE_H__
#define __FILE_H__

#include <type.h>

#define MAX_FD 10


#define NOMAL_FILE 0
#define TTY        1

typedef struct{
    uint8  isused;
    uint16 dev;
    uint32 indexno;
    uint32 offset;
    uint16 startnode;
    uint16 curnode;
    uint32 length;
}__attribute__ ((packed)) fileindex;

void initfs();
int file_open(fileindex *file,const char *path, int flags, ...);
int file_read(fileindex *file,void *buff,size_t len);
off_t file_lseek(fileindex *file,off_t offset, int whence);
int file_write(fileindex *file,const void *ptr,size_t len);  //doesn't Implement


#endif
