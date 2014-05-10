#include <process.h>
#include <common.h>
#include <memory.h>
#include <schedule.h>
#include <errno.h>
#include <unistd.h>

void cleanup(pid_t pid)
{
    ptable *pdt = mappage(PROTABLE[pid].pdt);
    int i, j;
    for (i = USEPAGE; i < USEENDP; ++i) {
        if (pdt[i].P) {
            ptable *pte = mappage(pdt[i].base);
            for (j = 0; j < ENDPAGE; ++j) {
                if (pte[j].P) {
                    if (pte[j].AVL) {
                        devpage(pdt[i].base, j);
                    } else {
                        freempage(pte[j].base);
                    }
                    pte[j].P = 0;
                }
            }
            unmappage(pte);
            freempage(pdt[i].base);
            pdt[i].P = 0;
        }
    }

    ptable *pte = mappage(pdt[USEENDP].base);
    for (j = 0; j < USEENDP; ++j) {
        if (pte[j].P) {
            if (pte[j].AVL) {
                devpage(pdt[i].base, j);
            } else {
                freempage(pte[j].base);
            }
            pte[j].P = 0;
        }
    }
    unmappage(pte);
    unmappage(pdt);
}


void sys_exit(int status)
{
    int i;
    for(i=0;i<MAX_PROCESS;++i){
        close(i);
    }
    
    cli();
    cleanup(curpid);
    
    PROTABLE[curpid].ret = status;
    PROTABLE[curpid].status=deaded;
    
    
    if (PROTABLE[PROTABLE[curpid].ppid].status == waiting) {
        unblock(PROTABLE[curpid].ppid);
    }

    for (i = 0; i < MAX_PROCESS; ++i) {
        if (PROTABLE[i].ppid == curpid) {
            PROTABLE[i].ppid = 1;
            if (PROTABLE[i].status == deaded && PROTABLE[1].status == waiting) {
                unblock(1);
            }
        }
    }
    schedule();

}

void cleanchild(pid_t pid)
{
    ptable *pdt = mappage(PROTABLE[pid].pdt);
    ptable *pte = mappage(pdt[USEENDP].base);
    freempage(pte[USEENDP].base);
    pte[USEENDP].P = 0;
    unmappage(pte);
    unmappage(pdt);
    freempage(PROTABLE[pid].pdt);
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
        block(curpid);
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
