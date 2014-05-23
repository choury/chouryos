#include <common.h>
#include <process.h>
#include <errno.h>
#include <schedule.h>
#include <malloc.h>
#include <memory.h>


#define BLOCK    1
#define COMPELET 2



struct wmlist{
    pid_t from;
    pid_t to;
    void *buff;
    size_t left;
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
    PROTABLE[curpid].file[fd].target.dest=pid;
    return fd;
}


//from=0 则表示接受所有进程发来的消息
int msg_read(pid_t from,void *buff,size_t len,uint32 flags){
    struct wmlist *tmp=wmhead.next;
    struct wmlist *ptmp=&wmhead;
    int left=len;
    cli();
    while(tmp){
        if(tmp->to==curpid && 
            (tmp->from == from || from == 0) && 
            (tmp->flags & COMPELET) == 0){
            size_t cpysize=MIN(left,tmp->left);
            umemcpy(curpid,buff,tmp->from,tmp->buff,cpysize);
            left -=cpysize;
            buff +=cpysize;
            tmp->left-=cpysize;
            tmp->buff+=cpysize;
            if((tmp->flags & BLOCK) == 0 || 
                tmp->left == 0){
                tmp->flags |= COMPELET;
                unblock(tmp->from);
            }
            if((flags & BLOCK) == 0 ||
                left == 0){
                return len-left;
            }
        }
        ptmp=tmp;
        tmp=tmp->next;
    }
    tmp=malloc(sizeof(struct wmlist));
    tmp->buff=buff;
    tmp->left=left;
    tmp->to=curpid;
    tmp->from=from;
    tmp->next=NULL;
    tmp->flags=flags;
    
    ptmp->next=tmp;

    sti();
    block(curpid,DMSG);
    cli();
    int ret=len-tmp->left;
    ptmp=&wmhead;
    while(ptmp->next!=tmp)ptmp=ptmp->next;
    ptmp->next=tmp->next;
    sti();
    free(tmp);
    return ret;
}

int msg_write(pid_t to,const void *buff,size_t len,uint32 flags){
    struct wmlist *tmp=wmhead.next;
    struct wmlist *ptmp=&wmhead;
    cli();
    int left=len;
    while(tmp){
        if((tmp->from==curpid || tmp->from==0) && 
            tmp->to == to && 
            (tmp->flags & COMPELET) == 0){
            size_t cpysize=MIN(len,tmp->left);
            umemcpy(tmp->to,tmp->buff,curpid,buff,cpysize);
            left -=cpysize;
            buff +=cpysize;
            tmp->left-=cpysize;
            tmp->buff+=cpysize;
            if((tmp->flags & BLOCK) == 0 || 
                tmp->left == 0){
                tmp->flags |= COMPELET;
                unblock(tmp->to);
            }
            if((flags & BLOCK) == 0 ||
                left == 0){
                return len-left;
            }
        }
        ptmp=tmp;
        tmp=tmp->next;
    }
    tmp=malloc(sizeof(struct wmlist));
    tmp->buff=(void *)buff;
    tmp->left=left;
    tmp->to=to;
    tmp->from=curpid;
    tmp->next=NULL;
    tmp->flags=flags;
    
    ptmp->next=tmp;
    
    sti();
    block(curpid,DMSG);
    cli();
    int ret=len-tmp->left;

    ptmp=&wmhead;
    while(ptmp->next!=tmp)ptmp=ptmp->next;
    ptmp->next=tmp->next;
        
    sti();
    free(tmp);
    return ret;
}