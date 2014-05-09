#include <stdio.h>
#include <unistd.h>

int socket(pid_t pid,int flags);

int main(){
    pid_t child;
    pid_t parent=getpid();
    if((child=fork())==0){
        int fd=socket(parent,0);
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
