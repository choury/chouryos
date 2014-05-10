#ifndef __FILE_H__
#define __FILE_H__

#include <type.h>
#include <time.h>

#define MAX_FD 10
#define MAX_DEV 10

typedef enum{
    NOMAL_FILE,TTY,MSG
}Filetype;


typedef struct{
    uint32      isused;
    Filetype    type;
    union{
        DEV         dev;
        pid_t       dest;
    }taget;
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
} filedes;





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
int file_open(filedes *file,const char *path, int flags);
int file_read(filedes *file,void *buff,size_t len);
off_t file_lseek(filedes *file,off_t offset, int whence);
int file_write(filedes *file,const void *ptr,size_t len);
int file_close(filedes *file);

#endif
