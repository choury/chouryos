#include <multiboot.h>
#include <chouryos.h>
#include <string.h>
#include <memory.h>

#define upto(x,y) ((x)+(y)-((((long)x)-1)%(y)+1))       //x对y向上取整 比如 upto(12,10）= 20
#define setavl(x) MMAP[(x)>>3] |=  (1<<((x)%8))          //将某页面设为可用(1为可用）
#define unsavl(x) MMAP[(x)>>3] &= ~(1<<((x)%8))          //将某页面设为不可用
#define isavl(x)  MMAP[(x)>>3] &   (1<<((x)%8))          //测试某页是否可用



#define RESPAGE   0x500                                 //前5M空间保留
#define MAXPAGE   0x100000                              //最多4G

void getmemmap(struct Bootinfo *boot){
    memset(charbuff,0,80*25*2);  //清屏
    memset(MMAP,0x00,MAXPAGE>>3);
    if(boot->flags&(1<<6)){
        struct memmap *map;
        for(map=boot->mmap_addr;
            (uint32)map<(uint32)boot->mmap_addr+boot->mmap_length;
            map=(struct memmap *)((uint32)map+map->size+sizeof(map->size))){
        printf("The first size:%d,addr:0x%X%08X,lenth:0x%X%08X,type:%d\n",
               map->size,
               map->base_addrh,
               map->base_addr,
               map->lengthh,
               map->length,
               map->type);
            if(map->type==1){
                uint8* tmpaddr=upto(map->base_addr,PAGESIZE);
                map->length-=(tmpaddr-map->base_addr);
                map->base_addr=tmpaddr;
                size_t count=map->length/PAGESIZE;
                size_t startp=(int)map->base_addr/PAGESIZE;
                int i;
                for(i=0;i<count;++i){
                    setavl(startp+i);
                }
                
            }
        }
    }else{
        putstring("Can't get memory map!\n");
    }
    memset(MMAP,0x00,RESPAGE>>3);
}

int getmpage(){
    int i;
    for(i=RESPAGE;i<MAXPAGE;++i){
        if(isavl(i)){
            unsavl(i);
            return i;
        }
    }
    printf("Can't get more memory!");
    return -1;
}


void pagecpy(uint32 dest,uint32 src){
    KINDEX[CPYDEST].base=dest;
    char *pdest=getvmaddr(0,CPYDEST);
    invlpg(pdest);
    
    KINDEX[CPYSRC].base=src;
    char *psrc=getvmaddr(0,CPYSRC);
    invlpg(psrc);
    
    memcpy(pdest,psrc,PAGESIZE);
}
