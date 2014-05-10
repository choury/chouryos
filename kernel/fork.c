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

    
    cli();
    PROTABLE[newpid].status=ready;
    PROTABLE[newpid].reg.eax=0;
    PROTABLE[newpid].pid=newpid;
    PROTABLE[newpid].ppid=curpid;
    PROTABLE[newpid].pdt=getmpage();                                  //创建新的页目录
    pagecpy(PROTABLE[newpid].pdt,PROTABLE[curpid].pdt);                  //父子进程的内存映射相同
    
    ptable *pdt_cur=mappage(PROTABLE[curpid].pdt);
    ptable *pdt_new=mappage(PROTABLE[newpid].pdt);
    
    for(i=USEPAGE;i<ENDPAGE;++i){
        if(pdt_cur[i].P){                                                     //给用户进程创建新的页框
            pdt_new[i].base=getmpage();   
            pagecpy(pdt_new[i].base,pdt_cur[i].base);
        
            ptable *pte_cur=mappage(pdt_cur[i].base);
            ptable *pte_new=mappage(pdt_new[i].base);
            
            int j;
            for(j=0;j<ENDPAGE;++j){
                if(i==USEPAGE && j==0){
                    pte_new[0].base=getmpage();
                    pagecpy(pte_new[0].base,pte_cur[0].base);
                    continue;
                }
                if(pte_cur[j].P){
                    sharepage(pdt_cur[i].base,j,pdt_new[i].base,j);               //写时复制
                    invlpg(getvmaddr(i,j));
                }
            }
            unmappage(pte_cur);
            unmappage(pte_new);
        }
    }
     
    ptable *pte_cks=mappage(pdt_cur[USEENDP].base); //复制栈
    ptable *pte_nks=mappage(pdt_new[USEENDP].base);
    
    
    devpage(pdt_new[USEENDP].base,1023);
    pte_nks[1023].base=getmpage();
    pagecpy(pte_nks[1023].base,pte_cks[1023].base);
    
    
    unmappage(pte_cks);
    unmappage(pte_nks);
    unmappage(pdt_cur);
    unmappage(pdt_new);
    sti();
    return newpid;
}
