#include <chouryos.h>

void schedule() {
    int i = curpid + 1;

    while ( i != curpid ) {
        if ( i == MAX_PROCESS ) {
            i = 1;
        }

        if ( PROTABLE[i].status == ready ) {
            curpid = i;
            return;
        }
        i++;
    }

    curpid = 0;
}
