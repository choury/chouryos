#include <chouryos.h>
#include <elf.h>
#include <fcntl.h>
#include <syscall.h>
#include <string.h>
#include <unistd.h>

#define MAX(x,y)    ((x)>(y)?(x):(y))

/*
 * execve
 * Transfer control to a new process.
 */



int sys_execve(char *name, char **argv, char **env) {
    fileindex fd;
    if(curpid==0){
        putstring("The process 0 can't call execve!\n");
        return -1;
    }
    if( file_open(&fd, name, O_RDONLY ) < 0 ){
        putstring("No such file!\n");
        return -1;
    }else{
        Elf32_Ehdr elf32_eh;
        Elf32_Phdr elf32_ph;
        if( file_read( &fd, &elf32_eh, sizeof(Elf32_Ehdr) ) == sizeof(Elf32_Ehdr) ){
            do{
                if(strncmp( ELFMAG, (const char*)elf32_eh.e_ident, SELFMAG ) != 0)
                    break;
                if(elf32_eh.e_ident[EI_CLASS] != ELFCLASS32)
                    break;
                if(elf32_eh.e_ident[EI_DATA] != ELFDATA2LSB)
                    break;
                if(elf32_eh.e_ident[EI_VERSION] != EV_CURRENT )
                    break;
                if(elf32_eh.e_type != ET_EXEC )
                    break;
                if(elf32_eh.e_machine != EM_386 )
                    break;
                if(elf32_eh.e_version != EV_CURRENT)
                    break;
                if(elf32_eh.e_phoff == 0)
                    break;
                int i;
                void *base=(void *)0x400000;
                void *heap=base;
                for(i=0;i<elf32_eh.e_phnum;++i){
                    file_lseek(&fd,elf32_eh.e_phoff+i*elf32_eh.e_phentsize,SEEK_SET);
                    printf("file.offset:%d,curnode:%d\n",fd.offset,fd.curnode);
                    if(file_read(&fd,&elf32_ph,elf32_eh.e_phentsize )== elf32_eh.e_phentsize ){
                        printf("ptype:%ud,pvaddr:%u,poffset:%u,pfilesz:%u,pmemsz:%u\n",
                               elf32_ph.p_type,elf32_ph.p_vaddr,elf32_ph.p_offset,elf32_ph.p_filesz,elf32_ph.p_memsz);
                        if(elf32_ph.p_type == PT_LOAD){
                            file_lseek(&fd,elf32_ph.p_offset,SEEK_SET);
                            file_read(&fd,base+elf32_ph.p_vaddr,elf32_ph.p_filesz);
                            heap=MAX(heap,base+elf32_ph.p_vaddr+elf32_ph.p_memsz);
                        }
                    }else{
                        file_close(&fd);
                        putstring("The file is broken!\n");
                        return -1;
                    }
                }
                PROTABLE[curpid].cdt.base0_23=(int)base;
                PROTABLE[curpid].cdt.base24_31=(int)base>>24;
                PROTABLE[curpid].ddt.base0_23=(int)base;
                PROTABLE[curpid].ddt.base24_31=(int)base>>24;
                PROTABLE[curpid].heap=heap;
                PROTABLE[curpid].base=base;
                PROTABLE[curpid].reg.eip=elf32_eh.e_entry;
                PROTABLE[curpid].reg.oesp=0xfffff;
                for(i=3; i<MAX_FD; i=i+1) {
                    PROTABLE[curpid].file[i].isused=0;              //关闭所有打开的文件
                }
                file_close(&fd);
                putstring("exec!\n");
                return 0;
            }while(0);
        }
        putstring("The file is not executable file!\n");
        file_close(&fd);
        return -1;
    }
}
