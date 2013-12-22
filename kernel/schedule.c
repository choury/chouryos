#include <chouryos.h>

void schedule() {
    int i = curpid + 1;

    while ( i != curpid ) {
        if ( i == MAX_PROCESS ) {
            i = 1;
        }

        if ( PROTABLE[i].status == ready ) {
            curpid = i;
            PROTABLE[i].status=running;
            return;
        }
        i++;
    }

    curpid = 0;
    PROTABLE[0].status=running;
}
