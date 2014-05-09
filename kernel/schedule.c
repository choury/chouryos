#include <common.h>
#include <schedule.h>

void TimerInitHandler() {
    outp( 0x20, 0x20 );
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
