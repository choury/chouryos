#include <chouryos.h>
#include <string.h>
#include <unistd.h>


int putstring(const char* s) {
    int i=strlen(s);
    write(1,s,i);
    return i;
}
