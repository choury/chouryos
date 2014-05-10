#include <common.h>
#include <process.h>
#include <errno.h>
#include <string.h>
#include <schedule.h>
#include <malloc.h>

#define MIN(x,y)    ((uint32)(x)<(uint32)(y)?(uint32)(x):(uint32)(y))

struct wmlist{
    pid_t from;
    pid_t to;
    void *buff;
    size_t len;
    uint32 flags;
    struct wmlist *next;
};

static struct wmlist *wmhead=NULL;

int sys_message(pid_t pid,uint32 flags){
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
    PROTABLE[curpid].file[fd].type=MSG;
    PROTABLE[curpid].file[fd].taget.dest=pid;
    return fd;
}

int msg_read(pid_t pid,void *buff,size_t len){
    struct wmlist *tmp=wmhead;
    struct wmlist *ptmp=tmp;
    cli();
    while(tmp){
        if(tmp->to==curpid && tmp->from == pid && tmp->flags == 0){
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
    tmp=malloc(sizeof(struct wmlist));
    tmp->buff=buff;
    tmp->len=len;
    tmp->to=curpid;
    tmp->from=pid;
    tmp->next=NULL;
    tmp->flags=0;
    if(wmhead == NULL){
        wmhead = tmp;
    }else{
        ptmp->next=tmp;
    }
    sti();
    block(curpid,DMSG);
    cli();
    int ret=tmp->len;
    if(wmhead == tmp){
        wmhead=NULL;
    }else{
        ptmp=wmhead;
        while(ptmp->next!=tmp)ptmp=ptmp->next;
        ptmp->next=tmp->next;
    }
    sti();
    free(tmp);
    return ret;
}

int msg_write(pid_t pid,const void *ptr,size_t len){
    struct wmlist *tmp=wmhead;
    struct wmlist *ptmp=tmp;
    cli();
    while(tmp){
        if(tmp->from==curpid && tmp->to == pid && tmp->flags == 0){
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
    tmp=malloc(sizeof(struct wmlist));
    tmp->buff=(void *)ptr;
    tmp->len=len;
    tmp->to=pid;
    tmp->from=curpid;
    tmp->next=NULL;
    tmp->flags=0;
    if(wmhead == NULL){
        wmhead = tmp;
    }else{
        ptmp->next=tmp;
    }
    sti();
    block(curpid,DMSG);
    cli();
    int ret=tmp->len;
    if(wmhead == tmp){
        wmhead=NULL;
    }else{
        ptmp=wmhead;
        while(ptmp->next!=tmp)ptmp=ptmp->next;
        ptmp->next=tmp->next;
    }
    sti();
    free(tmp);
    return ret;
}