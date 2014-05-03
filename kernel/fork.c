#include <syscall.h>
#include <chouryos.h>
#include <string.h>
#include <memory.h>

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
    cli();
    register_status *prs=(register_status*)(0xffffffff-sizeof(register_status));          //父进程系统栈压入了一个register_status的副本
    memcpy(PROTABLE+newpid,PROTABLE+curpid,sizeof(process));                              //复制进程控制块
    memcpy(&PROTABLE[newpid].reg,prs,sizeof(register_status));                            //复制寄存器值，从父进程系统栈

    PROTABLE[newpid].status=ready;
    PROTABLE[newpid].reg.eax=0;
    PROTABLE[newpid].pid=newpid;
    PROTABLE[newpid].ppid=curpid;
    PROTABLE[newpid].pdt=getmpage();                                  //创建新的页目录
    pagecpy(PROTABLE[newpid].pdt,PROTABLE[curpid].pdt);                  //父子进程的内存映射相同
    
    
    KINDEX[TMPINDEX0].base=PROTABLE[curpid].pdt;
    ptable *pdt_cur=getvmaddr(0,TMPINDEX0);
    invlpg(pdt_cur);
    
    KINDEX[TMPINDEX1].base=PROTABLE[newpid].pdt;
    ptable *pdt_new=getvmaddr(0,TMPINDEX1);
    invlpg(pdt_new);
    
    for(i=USEPAGE;i<ENDPAGE;++i){
        if(pdt_cur[i].P){                                                     //给用户进程创建新的页框
            pdt_new[i].base=getmpage();   
            pagecpy(pdt_new[i].base,pdt_cur[i].base);
            
            KINDEX[TMPINDEX2].base=pdt_cur[i].base;
            ptable *pte_cur=getvmaddr(0,TMPINDEX2);
            invlpg(pte_cur);
            
            KINDEX[TMPINDEX3].base=pdt_new[i].base;
            ptable *pte_new=getvmaddr(0,TMPINDEX3);
            invlpg(pte_new);
            
            int j;
            for(j=0;j<ENDPAGE;++j){
                if(pte_cur[j].P){
                    pte_cur[j].R_W=0;               //写时复制
                    pte_new[j].R_W=0;
                }
            }
        }
    }
    
    KINDEX[TMPINDEX2].base=pdt_cur[USEENDP].base;                              //复制栈  
    invlpg(getvmaddr(0,TMPINDEX2));  
    ptable *pte_cks=getvmaddr(0,TMPINDEX2);
    
    KINDEX[TMPINDEX3].base=pdt_new[USEENDP].base;
    invlpg(getvmaddr(0,TMPINDEX3));
    ptable *pte_nks=getvmaddr(0,TMPINDEX3);
    
    pte_nks[1022].base=getmpage();
    pte_nks[1022].R_W=1;
    pte_cks[1022].R_W=1;
    pagecpy(pte_nks[1022].base,pte_cks[1022].base);
    
    pte_nks[1023].base=getmpage();
    pte_nks[1023].R_W=1;
    pte_cks[1023].R_W=1;
    pagecpy(pte_nks[1023].base,pte_cks[1023].base);

    sti();
    return newpid;
}
