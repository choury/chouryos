#include <chouryos.h>
#include <schedule.h>

void TimerInitHandler() {
    outp( 0x20, 0x20 );
    schedule();
}


void schedule() {
    if(PROTABLE[1].status == ready){
        switch_to(1);
    }else{
        switch_to(0);
    }
/*    int i;
    for(i=curpid+1;i!=curpid;++i){
        if(i==MAX_PROCESS)i=0;
        if(PROTABLE[i].status == ready){
            switch_to(i);
        }
    }*/
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
