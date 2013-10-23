#include <sys/types.h>

#ifndef __STRING_H__
#define __STRING_H__

/* we use this so that we can do without the ctype library */
#define isdigit(c) ((c) >= '0' && (c) <= '9')
#define isxdigit(c) (((c) >= '0' && (c) <= '9') ||\
                     ((c) >= 'A' && (c) <= 'F') ||\
                     ((c) >= 'a' && (c) <= 'f'))
#define islower(c)  ((c)>='a' && (c)<='z')
#define toupper(c)  ((c)-32)

int strlen(const char *s) ;
void* memcpy(void *dest,const void *src,size_t n) ;


#endif
