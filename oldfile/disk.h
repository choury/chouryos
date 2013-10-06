#ifndef __DISK_H__
#define __DISK_H__

int  readsector(int sector,char *buff);
int  writesector(int sector,char *buff);


#endif