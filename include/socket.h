#ifndef __SOCKET_H__
#define __SOCKET_H__

int socket_read(filedes *file,void *buff,size_t len);
int socket_write(filedes *file,const void *ptr,size_t len);

#endif