#include <common.h>
#include <elf.h>
#include <unistd.h>
#include <string.h>
#include <memory.h>
#include <file.h>
#include <process.h>
#include <fcntl.h>
#include <errno.h>





/*
 * execve
 * Transfer control to a new process.
 */

int sys_execve(char *name, char *const argv[], char *const env[])
{
    int fd;
    if (curpid == 0) {
        printf("The process 0 can't call execve!\n");
        return -1;
    }
    if ((fd = open(name, O_RDONLY)) < 0) {
        errno=ENOENT;
        return -1;
    } else {
        Elf32_Ehdr elf32_eh;
        Elf32_Phdr elf32_ph;
        if (read(fd, &elf32_eh, sizeof(Elf32_Ehdr)) == sizeof(Elf32_Ehdr)) {
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
                
                ptable *pde = mappage(PROTABLE[curpid].pde);
                int i, j;
                for (i = USEPAGE; i < USEENDP; ++i) {
                    if (pde[i].P) {                                                         //关闭所有共享页面
                        ptable *pte = mappage(pde[i].base);
                        for (j = 0; j < ENDPAGE; ++j) {
                            if(i==USEPAGE && j==0){
                                continue;
                            }
                            if (pte[j].P) {
                                if (pte[j].AVL) {
                                    devpage(pde[i].base, j);
                                } else {
                                    freempage(pte[j].base);
                                }
                                pte[j].P = 0;
                            }
                        }
                        unmappage(pte);
                    }
                }
                
                
                int envpage=getmpage();
                struct pinfo *buff=mappage(envpage);
                char *pbuff=(char*)buff;
        
                int argc = 0;
                int envc = 0;
                while (argv[argc])argc++;
                while (env[envc++]);
                buff->argc = argc;
                buff->argv = (char **)((uint32)PINF + sizeof(struct pinfo));
                buff->env = (char **)((uint32)(buff->argv)+argc*sizeof(void *));
                buff->endp=buff->argv + (argc+envc) * sizeof(void *);
                pbuff=(char *)((uint32)buff+sizeof(struct pinfo)+(argc+envc)*sizeof(void *));
                
                char **bargv=(char **)((uint32)buff + sizeof(struct pinfo));
                for (i = 0; i < argc; ++i) {
                    bargv[i] = (char *)((uint32)PINF + (uint32)pbuff - (uint32)buff);
                    pbuff += strlen(strcpy(pbuff, argv[i])) + 1;
                }
                
                char **benv = (char **)((uint32)bargv+argc*sizeof(void *));
                for (i = 0; i < envc - 1; ++i) {
                    benv[i] = (char *)((uint32)PINF + (uint32)pbuff - (uint32)buff);;
                    pbuff += strlen(strcpy(pbuff, env[i])) + 1;
                }
                benv[envc - 1] = NULL;
                buff->endp=(char *)((uint32)PINF + (uint32)pbuff - (uint32)buff);
                
                uint8 *heap = (void *)USECODE;
                for (i = 0; i < elf32_eh.e_phnum; ++i) {
                    lseek(fd, elf32_eh.e_phoff + i * elf32_eh.e_phentsize, SEEK_SET);
                    if (read(fd, &elf32_ph, elf32_eh.e_phentsize) == elf32_eh.e_phentsize) {
                        if (elf32_ph.p_type == PT_LOAD) {
                            int count = 0;

                            for (count = getpagec(elf32_ph.p_vaddr);
                                    count <= getpagec(elf32_ph.p_vaddr + elf32_ph.p_memsz);
                                    count ++) {
                                if (pde[count].P == 0) {
                                    SETPT(pde[count],getmpage(),1);
                                    ptable *pte = mappage(pde[count].base);
                                    memset(pte, 0, PAGESIZE);
                                    unmappage(pte);
                                }
                            }
                            ptable *pte = mappage(pde[getpagec(elf32_ph.p_vaddr)].base);

                            for (count = getpagei(elf32_ph.p_vaddr);
                                    count <= getpagei(elf32_ph.p_vaddr + elf32_ph.p_memsz);
                                    count ++) {
                                if (pte[count].P == 0) {
                                    SETPT(pte[count],getmpage(),1);
                                }
                            }
                            unmappage(pte);
                            lseek(fd, elf32_ph.p_offset, SEEK_SET);
                            read(fd, (void *)elf32_ph.p_vaddr, elf32_ph.p_filesz);
                            heap = (void *)MAX((uint32)heap, elf32_ph.p_vaddr + elf32_ph.p_memsz);
                        }
                    } else {
                        close(fd);
                        unmappage(pde);
                        _exit(ENOEXEC);
                    }
                }
                unmappage(pde);
                
                register_status *prs = (register_status *)(0xffffffff - sizeof(register_status));
                PROTABLE[curpid].heap = heap;
                PROTABLE[curpid].reg.eip = elf32_eh.e_entry;
                prs->eip = elf32_eh.e_entry;
                PROTABLE[curpid].reg.oesp = 0xffffffff - KSL;
                prs->oesp = 0xffffffff - KSL;
                for (i = 3; i < MAX_FD; i = i + 1) {
                    close(i);            //关闭所有打开的文件
                }
                
                memcpy(PINF,buff,(uint32)pbuff-(uint32)buff);
                unmappage(buff);
                freempage(envpage);
                
                invlapg();
                return 0;
            } while (0);
        }

        close(fd);
        errno=ENOEXEC;
        return -1;
    }
}
