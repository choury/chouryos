#include <common.h>
#include <file.h>
#include <process.h>
#include <errno.h>
#include <string.h>
#include <schedule.h>
#include <malloc.h>

#define MIN(x,y)    ((uint32)(x)<(uint32)(y)?(uint32)(x):(uint32)(y))

struct msglist{
    pid_t from;
    pid_t to;
    void *buff;
    size_t len;
    uint32 flags;
    struct msglist *next;
};

static struct msglist *msghead=NULL;

int sys_socket(pid_t pid,uint32 flags){
    int i;
    int fd=-1;
    for(i=0;i<MAX_FD;i++){
        if(!PROTABLE[curpid].file[i].isused){
            fd=i;
            break;
        }
    }
    if(fd<0){
        errno=EMFILE;
        return -1;
    }
    
    if(PROTABLE[pid].status == unuse){
        errno=ESRCH;
        return -1;
    }
    
    PROTABLE[curpid].file[fd].isused=1;
    PROTABLE[curpid].file[fd].type=SOCKET;
    PROTABLE[curpid].file[fd].taget.dest=pid;
    return fd;
}

int socket_read(filedes *file,void *buff,size_t len){
    struct msglist *tmp=msghead;
    struct msglist *ptmp=tmp;
    cli();
    while(tmp){
        if(tmp->to==curpid && tmp->from == file->taget.dest && tmp->flags == 0){
            size_t ret=MIN(len,tmp->len);
            memcpy(buff,tmp->buff,ret);
            tmp->len=ret;
            tmp->flags=1;
            unblock(tmp->from);
            return ret;
        }
        ptmp=tmp;
        tmp=tmp->next;
    }
    tmp=malloc(sizeof(struct msglist));
    tmp->buff=buff;
    tmp->len=len;
    tmp->to=curpid;
    tmp->from=file->taget.dest;
    tmp->next=NULL;
    tmp->flags=0;
    if(msghead == NULL){
        msghead = tmp;
    }else{
        ptmp->next=tmp;
    }
    sti();
    block(curpid);
    cli();
    int ret=tmp->len;
    if(msghead == tmp){
        msghead=NULL;
    }else{
        ptmp=msghead;
        while(ptmp->next!=tmp)ptmp=ptmp->next;
        ptmp->next=tmp->next;
    }
    sti();
    free(tmp);
    return ret;
}

int socket_write(filedes *file,const void *ptr,size_t len){
    struct msglist *tmp=msghead;
    struct msglist *ptmp=tmp;
    cli();
    while(tmp){
        if(tmp->from==curpid && tmp->to == file->taget.dest && tmp->flags == 0){
            size_t ret=MIN(len,tmp->len);
            memcpy(tmp->buff,ptr,ret);
            tmp->len=ret;
            tmp->flags=1;
            unblock(tmp->to);
            return ret;
        }
        ptmp=tmp;
        tmp=tmp->next;
    }
    tmp=malloc(sizeof(struct msglist));
    tmp->buff=(void *)ptr;
    tmp->len=len;
    tmp->to=file->taget.dest;
    tmp->from=curpid;
    tmp->next=NULL;
    tmp->flags=0;
    if(msghead == NULL){
        msghead = tmp;
    }else{
        ptmp->next=tmp;
    }
    sti();
    block(curpid);
    cli();
    int ret=tmp->len;
    if(msghead == tmp){
        msghead=NULL;
    }else{
        ptmp=msghead;
        while(ptmp->next!=tmp)ptmp=ptmp->next;
        ptmp->next=tmp->next;
    }
    sti();
    free(tmp);
    return ret;
}