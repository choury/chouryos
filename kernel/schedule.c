#include <common.h>
#include <schedule.h>
#include <malloc.h>
#include <signal.h>
#include <unistd.h>

void TimerInitHandler() {
    outp( 0x20, 0x20 );
    if(PROTABLE[curpid].sighead.next){
        struct siglist *tmp=PROTABLE[curpid].sighead.next;
        switch(tmp->sig){
        case SIGTERM:
            _exit(1);
            break;
        }
        PROTABLE[curpid].sighead.next=tmp->next;
        free(tmp);
    }
    schedule();
}


void schedule() {
    int i;
    if(curpid==0){
        for(i=1;i<MAX_PROCESS;++i){
            if(PROTABLE[i].status == ready){
                switch_to(i);
                return;
            }
        }
    }else{
        for(i=curpid+1;i!=curpid;++i){
            if(PROTABLE[i].status == ready){
                switch_to(i);
                return;
            }
            if(i==MAX_PROCESS-1)i=0;
        }
    }
    if(PROTABLE[curpid].status != running){
        switch_to(0);
    }
}


void block(pid_t pid,DEV waitfor){
    if(pid ==0 || PROTABLE[pid].status == unuse)
        return;
    PROTABLE[pid].waitfor=waitfor;
    PROTABLE[pid].status=waiting;
    schedule();
}

void unblock(pid_t pid){
    if(PROTABLE[pid].status == unuse)
        return;
    PROTABLE[pid].status=ready;
}

void switch_to( pid_t pid ) {
    if ( pid == curpid )
        return;
    
    cli();
    if ( PROTABLE[curpid].status == running ) {
        PROTABLE[curpid].status = ready;
    }

    pid_t last = curpid;
    curpid = pid;
    PROTABLE[curpid].status = running;
    do_switch_to( &( PROTABLE[last].pid ),PROTABLE+curpid,PROTABLE[curpid].pdt);
    sti();
}
