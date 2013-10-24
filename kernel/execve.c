#include <syscall.h>
#include <chouryos.h>
#include <elf.h>
#include </home/housir/myos/newlib-i386/include/string.h>
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
	int fd = sys_open( name, O_RDONLY );
	if( fd == -1 ) // fail to open file
	{
		return -1;
	}
	else
	{
		struct Elf32_Ehdr elf32;
		unsigned char ch[4] = {0x7F,'E', 'L', 'F'};
		unsigned char c;
		int index = 0;
		int n = sys_read( fd, elf32.e_ident, 8 );
		if( n == 8 && strcmp( ch, elf32.e_ident, 4 ) == 0 )
		{
			n = sys_read( fd, c, 1 ); // 第九个字节，currently unused
			n = sys_read( fd, elf32.e_type, 2 ); // 1, 2, 3, 4 specify whether the object is relocatable, executable, shared, or core, respectively.
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
			n = sys_read( fd, elf32.e_machine, 2 );
			n = sys_read( fd, elf32.e_version, 4 ); // Set to 1 for the original version of ELF.
			n = sys_read( fd, elf32.e_entry, 4 ); // This is the memory address of the entry point from where the process starts executing*. This field is either 32 or 64 bits long depending on the format defined earlier
			n = sys_read( fd, elf32.e_phoff, 4 ); // Points to the start of the program header table. It usually follows the file header immediately making the offset 0x40 for 64-bit ELF executables.
			n = sys_read( fd, elf32.e_shoff, 4 ); // Points to the start of the section header table.
			n = sys_read( fd, elf32.e_flags, 4 ); // Interpretation of this field depends on the target architecture.
			n = sys_read( fd, elf32.e_ehsize, 2 ); // Contains the size of this header, normally 64 bytes for 64-bit and 52 for 32-bit format.
			n = sys_read( fd, elf32.e_phentsize, 2 ); // Contains the size of a program header table entry.
			n = sys_read( fd, elf32.e_phnum, 2 ss); // Contains the number of entries in the program header table.
			n = sys_read( fd, elf32.e_shentsize, 2 ); // Contains the size of a section header table entry.
			n = sys_read( fd, elf32.e_shnum, 2 ); // Contains the number of entries in the section header table.
			n = sys_read( fd, elf32.e_shstrndx, 2 ); // Contains index of the section header table entry that contains the section names.
		}
	/*	if( n == 1 && ch[0] == 0x7F )
		{
			elf32.e_ident[index] = 0x7F;
			n = sys_read( fd, ch, 3 );
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
		
		sys_close(fd);
	}
	errno = ENOMEM;
	return -1;
}
