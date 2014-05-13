#ifndef __MSG_H__
#define __MSG_H__


int msg_read(pid_t from,void *buff,size_t len,uint32 flags);
int msg_write(pid_t to,const void *ptr,size_t len,uint32 flags);

#endif