#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <common.h>

#define PAGESIZE 0x1000

int getmpage();             //取得一块可用页面，失败返回-1
int freempage(uint32 page);  //释放一个页面
#define getvmaddr(x,y) (void *)(((x)<<22)+((y)<<12))  //取得x页目框，y页目录的开始地址
#define getpagec(a) ((uint32)(a)>>22)                //取得a地址的页框号
#define getpagei(a) (((uint32)(a)&0x3fffff)>>12)      //取得a地址的页目录号
#define getpages(count,index) (((count)<<22)+((index)<<12))     //取得该页面的起始地址
void pagecpy(uint32 dest,uint32 src);   //复制两个页面内容

void *mappage(uint32 page);
void unmappage(const void *addr);

void sharepage(uint32 pageca, uint16 pageia, uint32 pagecb, uint16 pageib);
void devpage(uint32 pagec, uint16 pagei);

void umemcpy(pid_t dpid, void* dest, pid_t spid, const void* src, size_t len);

#endif