#include <type.h>

#ifndef __MULTIBOOT_H__
#define __MULTIBOOT_H__


/*
             +-------------------+
     0       | flags             |    (required)
             +-------------------+
     4       | mem_lower         |    (present if flags[0] is set)
     8       | mem_upper         |    (present if flags[0] is set)
             +-------------------+
     12      | boot_device       |    (present if flags[1] is set)
             +-------------------+
     16      | cmdline           |    (present if flags[2] is set)
             +-------------------+
     20      | mods_count        |    (present if flags[3] is set)
     24      | mods_addr         |    (present if flags[3] is set)
             +-------------------+
     28 - 40 | syms              |    (present if flags[4] or
             |                   |                flags[5] is set)
             +-------------------+
     44      | mmap_length       |    (present if flags[6] is set)
     48      | mmap_addr         |    (present if flags[6] is set)
             +-------------------+
     52      | drives_length     |    (present if flags[7] is set)
     56      | drives_addr       |    (present if flags[7] is set)
             +-------------------+
     60      | config_table      |    (present if flags[8] is set)
             +-------------------+
     64      | boot_loader_name  |    (present if flags[9] is set)
             +-------------------+
     68      | apm_table         |    (present if flags[10] is set)
             +-------------------+
     72      | vbe_control_info  |    (present if flags[11] is set)
     76      | vbe_mode_info     |
     80      | vbe_mode          |
     82      | vbe_interface_seg |
     84      | vbe_interface_off |
     86      | vbe_interface_len |
             +-------------------+

Each module structure is formatted as follows:

             +-------------------+
     0       | mod_start         |
     4       | mod_end           |
             +-------------------+
     8       | string            |
             +-------------------+
     12      | reserved (0)      |
             +-------------------+

If bit 4 in the ‘flags’ word is set, then the following fields 
in the Multiboot information structure starting at byte 28 are valid:

             +-------------------+
     28      | tabsize           |
     32      | strsize           |
     36      | addr              |
     40      | reserved (0)      |
             +-------------------+
If bit 5 in the ‘flags’ word is set, then the following fields in the 
Multiboot information structure starting at byte 28 are valid:

             +-------------------+
     28      | num               |
     32      | size              |
     36      | addr              |
     40      | shndx             |
             +-------------------+

             
If bit 6 in the ‘flags’ word is set, then the ‘mmap_*’ fields are valid, 
and indicate the address and length of a buffer containing a memory map of the machine provided by the bios. 
‘mmap_addr’ is the address, and ‘mmap_length’ is the total size of the buffer. 
The buffer consists of one or more of the following size/structure pairs 
(‘size’ is really used for skipping to the next pair):

             +-------------------+
     -4      | size              |
             +-------------------+
     0       | base_addr         |
     8       | length            |
     16      | type              |
             +-------------------+
where ‘size’ is the size of the associated structure in bytes,
which can be greater than the minimum of 20 bytes. 
‘base_addr’ is the starting address. ‘length’ is the size of the memory region in bytes. 
‘type’ is the variety of address range represented, 
where a value of 1 indicates available ram, 
and all other values currently indicated a reserved area.
The map provided is guaranteed to list all standard ram that should be available for normal use.

*/

struct loadmod{
    void * mod_start;
    void * mod_end;
    char * string;
    uint8  reserved;
}__attribute__ ((packed));

struct memmap{
    uint32 size;
    void *base_addr;
    void *base_addrh;
    uint32 length;
    uint32 lengthh;
    uint32 type;
}__attribute__ ((packed));

struct Bootinfo{
    uint32 flags;
    uint32 mem_lower;
    uint32 mem_upper;
    uint8  boot_device[4];
    char * cmdline;
    uint32 mods_count;
    struct loadmod * mods_addr;
    union{
        struct{
            uint32 tabsize;
            uint32 strsize;
            uint32 addr;
            uint32 reserved;
        }a_out;
        struct{
            uint32 num;
            uint32 size;
            uint32 addr;
            uint32 shndx;
        }elf;
    }syms;
    uint32 mmap_length;
    struct memmap * mmap_addr;
    uint32 drives_length;
    uint32 drives_addr;
    uint32 config_table;
    uint32 boot_loader_name;
    uint32 apm_table;
    uint32 vbe_control_info;
    uint32 vbe_mode_info;
    uint16 vbe_mode;
    uint16 vbe_interface_seg;
    uint16 vbe_interface_off;
    uint16 vbe_interface_len;
}__attribute__ ((packed));

void getmemmap(struct Bootinfo *boot);

#endif
