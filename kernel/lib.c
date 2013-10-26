#include <chouryos.h>


int* __errno() {
    return (int *)12000;
}
