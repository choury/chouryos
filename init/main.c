#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

int socket(pid_t pid,int flags);

int main(int argc,char **argv){
    pid_t child;
    if((child=fork())==0){
        while(1){
            char buff[100];
            char arg[100];
            char *nargv[]={buff,arg,NULL};
            printf("root@chouryos:/# ");
            scanf("%99s%99s",buff,arg);
            if(fork()==0){
                execve(buff,nargv,environ);
                return errno;
            }else{
                int ret;
                wait(&ret);
                if(ret==ENOENT){
                    printf("shell:command %s not found.\n",buff);
                }
            }
        }
    }else{
        while(1){
            int ret;
            child=wait(&ret);
            printf("%d exit:%d\n",child,ret);
        }
    }
    return 0;
}
