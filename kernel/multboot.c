#include <multiboot.h>
#include <chouryos.h>

void getmemmap(struct Bootinfo *boot){
    if(boot->flags&(1<<6)){
        struct memmap *map;
        for(map=boot->mmap_addr;
            (uint32)map<(uint32)boot->mmap_addr+boot->mmap_length;
            map=(struct memmap *)((uint32)map+map->size+sizeof(map->size)))
        printf("The first size:%d,addr:0x%X%08X,lenth:0x%X%08X,type:%d\n",
               map->size,
               map->base_addrh,
               map->base_addr,
               map->lengthh,
               map->length,
               map->type);
    }else{
        putstring("Can't get memory map!\n");
    }
}

