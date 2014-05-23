#include <common.h>
#include <process.h>
#include <errno.h>
#include <memory.h>
#include <elf.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


pid_t sys_loadmod(const char* filename)
{
    int fd;
    if ((fd = open(filename, O_RDONLY)) < 0) {
        errno = ENOENT;
        return -1;
    } else {
        Elf32_Ehdr elf32_eh;
        Elf32_Phdr elf32_ph;
        if (read(fd, &elf32_eh, sizeof(Elf32_Ehdr)) == sizeof(Elf32_Ehdr)) {
            do {
                if (strncmp(ELFMAG, (const char*)elf32_eh.e_ident, SELFMAG) != 0)
                    break;
                if (elf32_eh.e_ident[EI_CLASS] != ELFCLASS32)
                    break;
                if (elf32_eh.e_ident[EI_DATA] != ELFDATA2LSB)
                    break;
                if (elf32_eh.e_ident[EI_VERSION] != EV_CURRENT)
                    break;
                if (elf32_eh.e_type != ET_EXEC)
                    break;
                if (elf32_eh.e_machine != EM_386)
                    break;
                if (elf32_eh.e_version != EV_CURRENT)
                    break;
                if (elf32_eh.e_phoff == 0)
                    break;
                cli();
                int i;
                for (i = 1; i < MAX_PROCESS; ++i) {
                    if (PROTABLE[i].status == unuse)break;
                }

                if (i == MAX_PROCESS) {
                    errno = EAGAIN;
                    return -1;
                }
                int newpid = i;
                
                PROTABLE[newpid].status = waiting;
                PROTABLE[newpid].reg.oesp = 0xffffffff-KSL;
                PROTABLE[newpid].reg.ss = MDATA_DT;
                PROTABLE[newpid].reg.cs = MCODE_DT;
                PROTABLE[newpid].reg.ds = MDATA_DT;
                PROTABLE[newpid].reg.es = MDATA_DT;
                PROTABLE[newpid].reg.fs = VGA_DT;
                PROTABLE[newpid].reg.gs = MDATA_DT;
                PROTABLE[newpid].reg.eflags =0x1202;
                PROTABLE[newpid].pid = newpid;
                PROTABLE[newpid].ppid = 1;
                PROTABLE[newpid].pde = getmpage();                              //创建新的页目录
                PROTABLE[newpid].sighead.next = NULL;
                pagecpy(PROTABLE[newpid].pde, PROTABLE[1].pde);                 //复制1号进程的地址空间
                
                PROTABLE[newpid].file[0].isused = 0;             //no standard input
                PROTABLE[newpid].file[1].isused = 1;             //for standard output
                PROTABLE[newpid].file[1].type = TTY;
                PROTABLE[newpid].file[2].isused = 1;             //for standard errer
                PROTABLE[newpid].file[2].type = TTY;
                for (i = 3; i < MAX_FD; i = i + 1) {
                    PROTABLE[newpid].file[i].isused = 0;
                }
                sti();
                
                ptable* pde = mappage(PROTABLE[newpid].pde);

                for (i = USEPAGE; i < ENDPAGE; ++i) {
                    if (pde[i].P) {                                             //关闭所有页面
                        ptable* pte = mappage(pde[i].base);
                        int j;
                        for (j = 0; j < ENDPAGE; ++j) {
                            if ((i == USEPAGE && j == 0)||
                                (i == USEENDP && j == USEENDP)){
                                pte[j].base = getmpage();
                                pte[j].U_S = 0;
                                continue;
                            }
                            if (pte[j].P) {
                                pte[j].P = 0;
                            }
                        }
                        unmappage(pte);
                    }
                }


                uint8* heap = (void*)USECODE;
                for (i = 0; i < elf32_eh.e_phnum; ++i) {
                    lseek(fd, elf32_eh.e_phoff + i * elf32_eh.e_phentsize, SEEK_SET);
                    if (read(fd, &elf32_ph, elf32_eh.e_phentsize) == elf32_eh.e_phentsize) {
                        if (elf32_ph.p_type == PT_LOAD) {
                            int addr=elf32_ph.p_vaddr;
                            int left =elf32_ph.p_filesz;
                            lseek(fd, elf32_ph.p_offset, SEEK_SET);
                            while(left){
                                int count=getpagec(addr);
                                int index=getpagei(addr);
                                ptable *pte;
                                if (pde[count].P == 0) {
                                    SETPT(pde[count],getmpage(),0);
                                    pte = mappage(pde[count].base);
                                    memset(pte, 0, PAGESIZE);
                                }else{
                                    pte = mappage(pde[count].base);
                                }
                                if (pte[index].P == 0) {
                                    SETPT(pte[index],getmpage(),0);
                                }
                                void *buff=mappage(pte[index].base);
                                unmappage(pte);
                                int len=MIN(left,(addr&0xfffff000)+PAGESIZE-addr);
                                read(fd, buff+(addr-(addr&0xfffff000)), len);
                                left -=len;
                                addr +=len;
                                unmappage(buff);
                            }
                            heap = (void*)MAX((uint32)heap, elf32_ph.p_vaddr + elf32_ph.p_memsz);
                        }
                    } else {                        
                        close(fd);
                        unmappage(pde);
                        errno = ENOEXEC;
                        return -1;
                    }
                }
                
                close(fd);
                unmappage(pde);
                

                PROTABLE[newpid].heap = heap;
                PROTABLE[newpid].reg.eip = elf32_eh.e_entry;
                PROTABLE[newpid].status = ready;
                
                errno=ENOEXEC;
                return newpid;
            } while (0);
        }
        close(fd);
        errno = ENOEXEC;
        return -1;
    }
}

