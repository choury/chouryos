#include <common.h>
#include <process.h>
#include <errno.h>
//#include <string.h>
#include <schedule.h>
#include <malloc.h>
#include <memory.h>


struct wmlist{
    pid_t from;
    pid_t to;
    void *buff;
    size_t len;
    uint32 flags;
    struct wmlist *next;
};

static struct wmlist wmhead={0,0,NULL,0,0,NULL};

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
    PROTABLE[curpid].file[fd].flags=flags;
    PROTABLE[curpid].file[fd].taget.dest=pid;
    return fd;
}


//from=0 则表示接受所有进程发来的消息
int msg_read(pid_t from,void *buff,size_t len){
    struct wmlist *tmp=wmhead.next;
    struct wmlist *ptmp=&wmhead;
    cli();
    while(tmp){
        if(tmp->to==curpid && 
            (tmp->from == from || from == 0) && 
            tmp->flags == 0){
            size_t ret=MIN(len,tmp->len);
            umemcpy(curpid,buff,tmp->from,tmp->buff,ret);
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
    tmp->from=from;
    tmp->next=NULL;
    tmp->flags=0;
    
    ptmp->next=tmp;

    sti();
    block(curpid,DMSG);
    cli();
    int ret=tmp->len;
    
    ptmp=&wmhead;
    while(ptmp->next!=tmp)ptmp=ptmp->next;
    ptmp->next=tmp->next;
    
    sti();
    free(tmp);
    return ret;
}

int msg_write(pid_t to,const void *ptr,size_t len){
    struct wmlist *tmp=wmhead.next;
    struct wmlist *ptmp=&wmhead;
    cli();
    while(tmp){
        if(tmp->from==curpid && tmp->to == to && tmp->flags == 0){
            size_t ret=MIN(len,tmp->len);
            umemcpy(tmp->to,tmp->buff,curpid,ptr,ret);
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
    tmp->to=to;
    tmp->from=curpid;
    tmp->next=NULL;
    tmp->flags=0;
    
    ptmp->next=tmp;
    
    sti();
    block(curpid,DMSG);
    cli();
    int ret=tmp->len;

    ptmp=&wmhead;
    while(ptmp->next!=tmp)ptmp=ptmp->next;
    ptmp->next=tmp->next;
        
    sti();
    free(tmp);
    return ret;
}