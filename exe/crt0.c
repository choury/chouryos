#include <unistd.h>
#include <common.h>
#include <process.h>

int main(int argc,char **argv);
pid_t _wait(int *status);

char ** env;

int *__errno()
{
    return &(PENV->errno);
}

void start(){
    env=PENV->env;
    _exit(main(PENV->argc,PENV->argv));
}

pid_t wait(int *status){
    return _wait(status);
}