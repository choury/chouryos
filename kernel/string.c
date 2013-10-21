#include <string.h>

int strlen(const char *s) {
    int i=0;
    while(*s) {
        s++;
        i++;
    }
    return i;
}

void* memcpy(void *dest,const void *src,size_t n){
    int i;
    for(i=0;i<n;++i){
        ((char *)dest)[i]=((char *)src)[i];
    }
    return dest;
}
