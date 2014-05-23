#include <common.h>
#include <string.h>
#include <memory.h>
#include <process.h>
#include <errno.h>


/*
 fork
 Create a new process
 */

int sys_fork() {
    int i,newpid;
    
    cli();
    for(i=1;i<MAX_PROCESS;++i){
        if(PROTABLE[i].status==unuse)break;
    }
    
    if(i==MAX_PROCESS){
        errno = EAGAIN;
        return -1;
    }
    
    newpid=i;
    register_status *prs=(register_status*)(0xffffffff-sizeof(register_status));          //父进程系统栈压入了一个register_status的副本
    memcpy(PROTABLE+newpid,PROTABLE+curpid,sizeof(process));                              //复制进程控制块
    memcpy(&PROTABLE[newpid].reg,prs,sizeof(register_status));                            //复制寄存器值，从父进程系统栈

    
    PROTABLE[newpid].status=waiting;
    PROTABLE[newpid].reg.eax=0;
    PROTABLE[newpid].pid=newpid;
    PROTABLE[newpid].ppid=curpid;
    PROTABLE[newpid].pde=getmpage();                                  //创建新的页目录
    PROTABLE[newpid].sighead.next=NULL;                                 //清空未处理的信号
    pagecpy(PROTABLE[newpid].pde,PROTABLE[curpid].pde);                  //父子进程的内存映射相同
    
    sti();
    
    ptable *pde_cur=mappage(PROTABLE[curpid].pde);
    ptable *pde_new=mappage(PROTABLE[newpid].pde);
    
    for(i=USEPAGE;i<ENDPAGE;++i){
        if(pde_cur[i].P){                                                     //给用户进程创建新的页框
            pde_new[i].base=getmpage();   
            pagecpy(pde_new[i].base,pde_cur[i].base);
        
            ptable *pte_cur=mappage(pde_cur[i].base);
            ptable *pte_new=mappage(pde_new[i].base);
            
            int j;
            for(j=0;j<ENDPAGE;++j){
                if(i==USEPAGE && j==0){
                    pte_new[0].base=getmpage();
                    pagecpy(pte_new[0].base,pte_cur[0].base);
                    continue;
                }
                if(pte_cur[j].P){
                    sharepage(pde_cur[i].base,j,pde_new[i].base,j);               //写时复制
                    invlpg(getvmaddr(i,j));
                }
            }
            unmappage(pte_cur);
            unmappage(pte_new);
        }
    }
     
    ptable *pte_cks=mappage(pde_cur[USEENDP].base); //复制栈
    ptable *pte_nks=mappage(pde_new[USEENDP].base);
    
    
    devpage(pde_new[USEENDP].base,USEENDP);
    pte_nks[USEENDP].base=getmpage();
    pagecpy(pte_nks[USEENDP].base,pte_cks[USEENDP].base);
    
    
    unmappage(pte_cks);
    unmappage(pte_nks);
    unmappage(pde_cur);
    unmappage(pde_new);
    
    PROTABLE[newpid].status=ready;
    return newpid;
}
