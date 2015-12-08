#include <process.h>
#include <common.h>
#include <memory.h>
#include <schedule.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>

void clearsiglist(pid_t pid){
    struct siglist *tmp=PROTABLE[pid].sighead.next;
    while(tmp){
        struct siglist *ttmp=tmp->next;
        free(tmp);
        tmp=ttmp;
    }
}


void cleanup(pid_t pid)
{
    clearsiglist(pid);
    ptable *pde = mappage(PROTABLE[pid].pde);
    int i, j;
    for (i = USEPAGE; i < USEENDP; ++i) {
        if (pde[i].P) {
            ptable *pte = mappage(pde[i].base);
            for (j = 0; j < ENDPAGE; ++j) {
                if (pte[j].P) {
                    if (pte[j].AVL) {
                        devpage(pde[i].base, j);
                    } else {
                        freempage(pte[j].base);
                    }
                    pte[j].P = 0;
                }
            }
            unmappage(pte);
            freempage(pde[i].base);
            pde[i].P = 0;
        }
    }

    ptable *pte = mappage(pde[USEENDP].base);
    for (j = 0; j < USEENDP; ++j) {
        if (pte[j].P) {
            if (pte[j].AVL) {
                devpage(pde[i].base, j);
            } else {
                freempage(pte[j].base);
            }
            pte[j].P = 0;
        }
    }
    unmappage(pte);
    unmappage(pde);
}


void sys_exit(int status)
{
    if(curpid ==0){
        return;
    }
    int i;
    for(i=0;i<MAX_FD;++i){
        close(i);
    }
    
    cli();
    cleanup(curpid);
    
    PROTABLE[curpid].ret = status;
    PROTABLE[curpid].status=deaded;
    
    
    if (PROTABLE[PROTABLE[curpid].ppid].status == waiting && 
        PROTABLE[PROTABLE[curpid].ppid].waitfor== DCHILD
    ) {
        unblock(PROTABLE[curpid].ppid);
    }

    for (i = 0; i < MAX_PROCESS; ++i) {
        if (PROTABLE[i].ppid == curpid) {
            PROTABLE[i].ppid = 1;
            if (PROTABLE[i].status == deaded && 
                PROTABLE[1].status == waiting &&
                PROTABLE[1].waitfor== DCHILD
            ) {
                unblock(1);
            }
        }
    }
    schedule();

}

void cleanchild(pid_t pid)
{
    ptable *pde = mappage(PROTABLE[pid].pde);
    ptable *pte = mappage(pde[USEENDP].base);
    freempage(pte[USEENDP].base);
    pte[USEENDP].P = 0;
    unmappage(pte);
    unmappage(pde);
    freempage(PROTABLE[pid].pde);
    PROTABLE[pid].status = unuse;
}

/*
 wait
 Wait for a child process.
 */
pid_t sys_wait(int *status)
{
    int i, haschild = 0;
    for (i = 0; i < MAX_PROCESS; ++i) {
        if (PROTABLE[i].ppid == curpid) {
            haschild = 1;
            if (PROTABLE[i].status == deaded) {
                if (status) {
                    *status = PROTABLE[i].ret;
                }
                cleanchild(i);
                return i;
            }
        }
    }
    if (haschild) {
        block(curpid,DCHILD);
        for (i = 0; i < MAX_PROCESS; ++i) {
            if (PROTABLE[i].ppid == curpid && PROTABLE[i].status == deaded) {
                if (status) {
                    *status = PROTABLE[i].ret;
                }
                cleanchild(i);
                return i;
            }
        }
    }

    errno = ECHILD;
    return -1;
}
