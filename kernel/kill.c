#include <process.h>
#include <signal.h>
#include <errno.h>
#include <malloc.h>


/*
 kill
 Send a signal. Minimal implementation:
 */
int sys_kill(int pid, int sig) {
    if(PROTABLE[pid].status == unuse){
        errno=ESRCH;
        return -1;
    }
    struct siglist *tmp;
    struct siglist *ptmp=&PROTABLE[pid].sighead;
    while(ptmp->next)ptmp=ptmp->next;
    switch(sig){
    case SIGTERM:
        tmp=malloc(sizeof(struct siglist));
        tmp->sig=sig;
        tmp->next=NULL;
        ptmp->next=tmp;
        return 0;
    }
    errno = EINVAL;
    return (-1);
}

