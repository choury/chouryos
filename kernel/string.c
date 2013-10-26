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
    __asm__("push %%es\n"
            "mov %0,%%ecx\n"
            "mov %1,%%edi\n"
            "mov %2,%%esi\n"
            "mov %%ds,%%ax\n"
            "mov %%ax,%%es\n"
            "cld\n"
            "rep movsb\n"
            "pop %%es"
            : :"g"(n),"g"(dest),"g"(src):"eax","ecx","edi","esi");
    return dest;
}
