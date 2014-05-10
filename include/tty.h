#ifndef __TTY_H__
#define __TTY_H__

int tty_read(void *buff,size_t len);
int tty_write(const void *ptr,size_t len);

#endif