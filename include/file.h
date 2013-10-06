#ifndef __FILE_H__
#define __FILE_H__

#include <type.h>

#define MAX_FD 10

typedef struct{
    uint8  isused;
    uint16 indexno;
    uint32 offset;
    uint16 startnode;
    uint16 curnode;
    uint32 length;
}__attribute__ ((packed)) fileindex;

void initfs();
int file_open(const char *path, int flags, ...);
int file_read(int fd,void *buff,size_t len);
int file_write(int fd,const void *ptr,size_t len);  //doesn't Implement


#endif
