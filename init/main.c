#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>


int socket(pid_t pid, int flags);

int main(int argc, char **argv)
{
    pid_t child;
    if ((child = fork()) == 0) {
        while (1) {
            char commad[100];
            char *nargv[20];
            printf("root@chouryos:/# ");
            fflush(stdout);
            gets(commad);
            int flag = 1;
            int nargc = 0;
            int i;
            for (i = 0; commad[i]; ++i) {
                if (flag && commad[i] != ' ') {
                    nargv[nargc++] = commad + i;
                    flag=0;
                }
                if (commad[i] == ' '){
                    commad[i] = 0;
                    flag=1;
                }
            }
            if(nargc == 0)
                continue;
            nargv[nargc]=NULL;

            if (fork() == 0) {
                execve(nargv[0], nargv, environ);
                return errno;
            } else {
                int ret;
                wait(&ret);
                if (ret == ENOENT) {
                    printf("shell:command %s not found.\n", nargv[0]);
                }
            }
        }
    } else {
        while (1) {
            int ret;
            child = wait(&ret);
            printf("%d exit:%d\n", child, ret);
        }
    }
    return 0;
}
