#include <chouryos.h>
#include <elf.h>
#include <fcntl.h>
#include <syscall.h>
#include <string.h>
#include <unistd.h>
#include <memory.h>
#include <file.h>
#include <process.h>

#define MAX(x,y)    ((uint32)(x)>(uint32)(y)?(uint32)(x):(uint32)(y))

/*
 * execve
 * Transfer control to a new process.
 */

int sys_execve(char *name, char **argv, char **env)
{
    fileindex fd;
    if (curpid == 0) {
        putstring("The process 0 can't call execve!\n");
        return -1;
    }
    if (file_open(&fd, name, O_RDONLY) < 0) {
        putstring("No such file!\n");
        return -1;
    } else {
        Elf32_Ehdr elf32_eh;
        Elf32_Phdr elf32_ph;
        if (file_read(&fd, &elf32_eh, sizeof(Elf32_Ehdr)) == sizeof(Elf32_Ehdr)) {
            do {
                if (strncmp(ELFMAG, (const char *)elf32_eh.e_ident, SELFMAG) != 0)
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
                
                ptable *pdt = mappage(PROTABLE[curpid].pdt);
                int i,j;
                for(i=USEPAGE;i<ENDPAGE;++i){
                    if(pdt[i].P){                                                           //关闭所有共享页面
                        ptable *pte = mappage(pdt[i].base);
                        for(j=0;j<ENDPAGE;++j){
                            if(pte[j].AVL){
                                devpage(pdt[i].base,j);
                                pte[j].P=0;
                            }
                        }
                        unmappage(pte);
                    }
                }

                uint8 *heap = (void *)USECODE;

                for (i = 0; i < elf32_eh.e_phnum; ++i) {
                    file_lseek(&fd, elf32_eh.e_phoff + i * elf32_eh.e_phentsize, SEEK_SET);
                    if (file_read(&fd, &elf32_ph, elf32_eh.e_phentsize) == elf32_eh.e_phentsize) {
                        printf("ptype:%u,pvaddr:0x%X,poffset:0x%X,pfilesz:0x%X,pmemsz:0x%X\n",
                               elf32_ph.p_type, elf32_ph.p_vaddr, elf32_ph.p_offset, elf32_ph.p_filesz, elf32_ph.p_memsz);
                        if (elf32_ph.p_type == PT_LOAD) {
                            ptable *pte = mappage(pdt[getpagec(elf32_ph.p_vaddr)].base);
                            int count = 0;
                            for (count = getpagei(elf32_ph.p_vaddr);
                                    count <= getpagei(elf32_ph.p_vaddr + elf32_ph.p_memsz);
                                    count ++) {
                                if (pte[count].P == 0) {
                                    pte[count].base = getmpage();
                                    pte[count].PAT = 0;
                                    pte[count].D = 0;
                                    pte[count].A = 0;
                                    pte[count].AVL=0;
                                    pte[count].PCD = 0;
                                    pte[count].PWT = 0;
                                    pte[count].U_S = 1;
                                    pte[count].R_W = 1;
                                    pte[count].P = 1;
                                }
                            }
                            unmappage(pte);
                            file_lseek(&fd, elf32_ph.p_offset, SEEK_SET);
                            file_read(&fd, (void *)elf32_ph.p_vaddr, elf32_ph.p_filesz);
                            heap = (void *)MAX(heap, elf32_ph.p_vaddr + elf32_ph.p_memsz);
                        }
                    } else {
                        file_close(&fd);
                        putstring("The file is broken!\n");
                        unmappage(pdt);
                        return -1;
                    }
                }
                unmappage(pdt);
                register_status *prs = (register_status*)(0xffffffff-sizeof(register_status));
                PROTABLE[curpid].heap = heap;
                PROTABLE[curpid].reg.eip = elf32_eh.e_entry;
                prs->eip = elf32_eh.e_entry;
                PROTABLE[curpid].reg.oesp = 0xffffffff - KSL;
                prs->oesp = 0xffffffff - KSL;
                for (i = 3; i < MAX_FD; i = i + 1) {
                    PROTABLE[curpid].file[i].isused = 0;            //关闭所有打开的文件
                }
                file_close(&fd);
                return 0;
            } while (0);
        }
        putstring("The file is not executable file!\n");
        file_close(&fd);
        return -1;
    }
}
