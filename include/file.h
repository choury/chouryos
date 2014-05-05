#ifndef __FILE_H__
#define __FILE_H__

#include <type.h>
#include <time.h>


#define MAX_FD 10
#define MAX_DEV 10

typedef enum{
    NOMAL_FILE,TTY
}Filetype;

typedef enum{
    Floopy,HardDisk
}DEV;


typedef struct{
    uint32      isused;
    Filetype    type;
    DEV         dev;
    uint32      dirnode;
    uint32      indexno;
    uint32      offset;
    uint32      startnode;
    uint32      curnode;
    uint32      nodebytes;
    uint32      length;
    time_t      createtime;
    time_t      accesstime;
    time_t      updatetime;
} fileindex;

typedef struct{
    uint8  isused;
    DEV    dev;
    uint32 firstdatenode;
}supernode;

struct waitqueue{
    pid_t pid;
    void* buff;
    size_t len;
    DEV  dev;
};

void initfs();
int file_open(fileindex *file,const char *path, int flags);
int file_read(fileindex *file,void *buff,size_t len);
off_t file_lseek(fileindex *file,off_t offset, int whence);
int file_write(fileindex *file,const void *ptr,size_t len);
int file_close(fileindex *file);

#endif
