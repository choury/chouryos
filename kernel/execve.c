#include <chouryos.h>
#include <elf.h>
#include <fcntl.h>
#include <syscall.h>

/*
 * execve
 * Transfer control to a new process.
 */

static int strcmp( const char *str1, const char *str2, int length )
{
	while(length-- && *str1 && *str2 && *str1==*str2)
	{
		++str1;
		++str2;
	}
	return *str1-*str2;
}

static int copy( char *s, int first_index, int count, char *d  )
{
	int index = 0;
	while( count-- )
	{
		s[first_index++] = d[index++];
	}
	return 1;
}

int sys_execve(char *name, char **argv, char **env) {
	//TODO .....
	// fuck,今天要给周威当苦力了吗
    fileindex fd;
	if( file_open(&fd, name, O_RDONLY ) < 0 ) // fail to open file
	{
        putstring("No such file!\n");
		return -1;
	}
	else
	{
		Elf32_Ehdr elf32;
		unsigned char c;
		int index = 0;
		int n = file_read( &fd, &elf32, sizeof(Elf32_Ehdr) );
		if( n == sizeof(Elf32_Ehdr) && strcmp( "\x7f""ELF", (const char*)elf32.e_ident, 4 ) == 0 )
		{
#if 0
			n = file_read( &fd, &c, 1 ); // 第九个字节，currently unused
			n = file_read( &fd, elf32.e_type, 2 ); // 1, 2, 3, 4 specify whether the object is relocatable, executable, shared, or core, respectively.
			/*
			 * Specifies target instruction set architecture. Some examples are:
			 * Value	ISA
			 * 0x02		SPARC
			 * 0x03		x86
			 * 0x14		PowerPC
			 * 0x28		ARM
			 * 0x32		IA-64
			 * 0x3E		x86-64
			 * 0xB7		AArch64
			 */
			n = file_read( &fd, elf32.e_machine, 2 );
			n = file_read( &fd, elf32.e_version, 4 ); // Set to 1 for the original version of ELF.
			n = file_read( &fd, elf32.e_entry, 4 ); // This is the memory address of the entry point from where the process starts executing*. This field is either 32 or 64 bits long depending on the format defined earlier
			n = file_read( &fd, elf32.e_phoff, 4 ); // Points to the start of the program header table. It usually follows the file header immediately making the offset 0x40 for 64-bit ELF executables.
			n = file_read( &fd, elf32.e_shoff, 4 ); // Points to the start of the section header table.
			n = file_read( &fd, elf32.e_flags, 4 ); // Interpretation of this field depends on the target architecture.
			n = file_read( &fd, elf32.e_ehsize, 2 ); // Contains the size of this header, normally 64 bytes for 64-bit and 52 for 32-bit format.
			n = file_read( &fd, elf32.e_phentsize, 2 ); // Contains the size of a program header table entry.
			n = file_read( &fd, elf32.e_phnum, 2 ); // Contains the number of entries in the program header table.
			n = file_read( &fd, elf32.e_shentsize, 2 ); // Contains the size of a section header table entry.
			n = file_read( &fd, elf32.e_shnum, 2 ); // Contains the number of entries in the section header table.
			n = file_read( &fd, elf32.e_shstrndx, 2 ); // Contains index of the section header table entry that contains the section names.
#endif
		}
	/*	if( n == 1 && ch[0] == 0x7F )
		{
			elf32.e_ident[index] = 0x7F;
			n = file_read( &fd, ch, 3 );
			if( strcmp( ch, "ELF" ) == 0 )
			{
				copy( elf32.e_ident, "ELF", 1, 3 );
				if(
			}
			else
			{
				return -1;
			}
		}*/
		else
		{
			return -1;
		}
		
		file_close(&fd);
	}
	errno = ENOMEM;
	return -1;
}
