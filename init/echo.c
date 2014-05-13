#include <stdio.h>
#include <unistd.h>

int message(pid_t pid, int flags);

int main(int argc,char **argv){
/*    int i;
    for(i=1;i<argc;++i){
        printf("%s ",argv[i]);
    }*/
    int fd=message(0,0);
    while(1){
        char command[1024];
        int ret=read(fd,command,1024);
        write(STDOUT_FILENO,command,ret);
    }
    return 0;
}