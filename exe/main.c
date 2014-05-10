#include <stdio.h>
#include <unistd.h>
#include <errno.h>

int socket(pid_t pid,int flags);

int main(int argc,char **argv){
    pid_t child;
    pid_t parent=getpid();
    if((child=fork())==0){
        int fd=socket(parent,0);
        lseek(fd,1,SEEK_SET);
        printf("errno=%d\n",errno);
        char buff[20];
        read(fd,buff,20);
        printf(buff);
    }else{
        int fd=socket(child,0);
        write(fd,"hello child!\n",14);
        printf("sended!\n");
    }
    return 0;
}
