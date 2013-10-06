#include <syscall.h>
#include <chouryos.h>
#include <file.h>







int sys_open(const char *path, int flags, ...){
    return file_open(path,flags);
}
