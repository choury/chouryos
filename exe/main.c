#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

int socket(pid_t pid,int flags);

int main(int argc,char **argv){
    pid_t child;
    if((child=fork())==0){
        int fd=socket(getppid(),0);
        char buff[20];
        int ret=read(fd,buff,20);
        printf("read:%s:%d\n",buff,ret);
        return 2;
    }else{
        int fd=socket(child,0);
        int ret=write(fd,"hello child!",13);
        printf("sended:%d!\n",ret);
        while(1){
            int ret;
            child=wait(&ret);
            printf("%d exit:%d\n",child,ret);
        }
    }
    return 0;
}
