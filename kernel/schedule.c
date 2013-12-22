#include <chouryos.h>
#include <schedule.h>

void TimerInitHandler() {
    outp( 0x20, 0x20 );
    schedule();
}


void schedule() {
    if ( ( curpid == 0 ) && ( PROTABLE[1].status == ready ) ) {
        switch_to( 1 );
    } else {
        switch_to( 0 );
    }
}

void switch_to( u32 pid ) {
    if ( pid == curpid )
        return;

    if ( PROTABLE[curpid].status == running ) {
        PROTABLE[curpid].status = ready;
    }

    cli();
    u32 last = curpid;
    curpid = pid;
    PROTABLE[curpid].status = running;
    do_switch_to( &( PROTABLE[last].pid ), &( PROTABLE[curpid] ),PROTABLE[curpid].ldt);
    sti();
}
