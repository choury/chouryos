#include <chouryos.h>
#include <elf.h>
#include <fcntl.h>
#include <syscall.h>
#include <string.h>

/*
 * execve
 * Transfer control to a new process.
 */



int sys_execve(char *name, char **argv, char **env) {
    //TODO .....
    fileindex fd;
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

                putstring("exec!\n");
                return 0;
            }while(0);
        }
        putstring("The file is not executable file!\n");
        file_close(&fd);
        return -1;
    }
    errno = ENOMEM;
    return -1;
}
