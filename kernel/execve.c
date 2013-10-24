#include <syscall.h>
#include <chouryos.h>
#include <elf.h>

/*
 execve
 Transfer control to a new process.
 */
int sys_execve(char *name, char **argv, char **env) {
    //TODO .....
  // fuck,今天要给周威当苦力了吗
  int fd = sys_open( name );
  if( fd == -1 ) // fail to open file
  {
    exit(0);
  }
  else
  {
    struct Elf32_Ehdr elf32;
    sys_close(fd);
  }
  errno = ENOMEM;
  return -1;
}
