#include <stdio.h>
#include <unistd.h>

int message(pid_t pid, int flags);

int main(int argc,char **argv){
    int i;
    for(i=1;i<argc;++i){
        printf("%s ",argv[i]);
    }
    putchar('\n');
    return 0;
}