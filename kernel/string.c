#include <string.h>

int strlen(const char *s) {
    int i;
    __asm__("push %%es\n"
            "mov $0xffffffff,%%ecx\n"
            "mov %1,%%edi\n"
            "mov %%ds,%%eax\n"
            "mov %%eax,%%es\n"
            "xor %%eax,%%eax\n"
            "cld\n"
            "repne scasb\n"
            "sub %1,%%edi\n"
            "dec %%edi\n"
            "mov %%edi,%0\n"
            "pop %%es"
            :"=g"(i):"g"(s):"eax","ecx","edi"
    );
    return i;
}

void *memset(void *s,int c,size_t n){
    __asm__("push %%es\n"
            "mov %0,%%ecx\n"
            "mov %1,%%edi\n"
            "mov %2,%%eax\n"
            "mov %%ds,%%ax\n"
            "mov %%ax,%%es\n"
            "cld\n"
            "rep stosb\n"
            "pop %%es"
            ::"g"(n),"g"(s),"g"(c):"eax","ecx","edi");
    return s;
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
