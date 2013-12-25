#include <multiboot.h>
#include <chouryos.h>

void getmemmap(struct Bootinfo *boot){
    if(boot->flags&(1<<6)){
        printf("offset:%d\n",((uint32)&boot->mmap_addr-(uint32)boot));
        printf("The addr is 0x%X,the length is %d\n",boot->mmap_addr,boot->mmap_length);
        printf("The first addr:0x%X,lenth:%d,type:%d\n",boot->mmap_addr[0].base_addr,boot->mmap_addr[0].length,boot->mmap_addr[0].type);
        printf("The second addr:0x%X,lenth:%d,type:%d\n",boot->mmap_addr[1].base_addr,boot->mmap_addr[1].length,boot->mmap_addr[1].type);
        printf("The third  addr:0x%X,lenth:%d,type:%d\n",boot->mmap_addr[2].base_addr,boot->mmap_addr[2].length,boot->mmap_addr[2].type);
    }else{
        putstring("Can't get memory map!\n");
    }
}