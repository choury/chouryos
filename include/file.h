#ifndef __FILE_H__
#define __FILE_H__

#include <type.h>


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

struct  stat 
{
  DEV       st_dev;
  uint32    st_ino;
  uint32    st_mode;
  uint32    st_nlink;
  uid_t     st_uid;
  gid_t     st_gid;
  DEV       st_rdev;
  off_t     st_size;
  time_t    st_atime;
  long      st_spare1;
  time_t    st_mtime;
  long      st_spare2;
  time_t    st_ctime;
  long      st_spare3;
  long      st_blksize;
  long      st_blocks;
  long      st_spare4[2];
};

#define     S_IFMT   0170000 /* type of file */
#define     S_IFDIR  0040000 /* directory */
#define     S_IFCHR  0020000 /* character special */
#define     S_IFBLK  0060000 /* block special */
#define     S_IFREG  0100000 /* regular */
#define     S_IFLNK  0120000 /* symbolic link */
#define     S_IFSOCK 0140000 /* socket */
#define     S_IFIFO  0010000 /* fifo */


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
