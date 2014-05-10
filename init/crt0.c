#include <unistd.h>
#include <common.h>
#include <process.h>

int main(int argc,char **argv);
pid_t _wait(int *status);


int *__errno()
{
    return &(PINF->errno);
}

void start(){
    environ=PINF->env;
    _exit(main(PINF->argc,PINF->argv));
}

pid_t wait(int *status){
    return _wait(status);
}