#include <multiboot.h>
#include <chouryos.h>
#include <string.h>
#include <memory.h>

#define upto(x,y) ((x)+(y)-((((long)x)-1)%(y)+1))       //x对y向上取整 比如 upto(12,10）= 20
#define setavl(map,x) (map[(x)>>3] |=  (1<<((x)%8)))          //将某页面设为可用(1为可用）
#define unavl(map,x)  (map[(x)>>3] &= ~(1<<((x)%8)))          //将某页面设为不可用
#define isavl(map,x)  (map[(x)>>3] &   (1<<((x)%8)))          //测试某页是否可用



#define RESPAGE   0x500                                 //前5M空间保留
#define MAXPAGE   0x100000                              //最多4G

void getmemmap(struct Bootinfo *boot){
    memset(charbuff,0,80*25*2);  //清屏
    memset(MMAP,0,MAXPAGE>>3);
    memset(MTMAP,0xff,1024>>8);
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
                    setavl(MMAP,startp+i);
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
        if(isavl(MMAP,i)){
            unavl(MMAP,i);
            return i;
        }
    }
    printf("getmpage:Can't get more memory!\n");
    return -1;
}

void freempage(uint32 page){
    if(!isavl(MMAP,page)){
       setavl(MMAP,page); 
    }else{
        printf("freempage:The page hasn't used!\n");
    }
    return;
}


void * mappage(uint32 page){
    int i;
    for(i=0;i<1024;++i){
        if(isavl(MTMAP,i)){
            unavl(MTMAP,i);
            break;
        }
    }
    TMPMAP[i].base=page;
    TMPMAP[i].P=1;
    void *addr=getvmaddr(MAPINDEX,i);
    invlpg(addr);
    return addr;
}

void unmappage(void *addr){
    if(getpagei(addr) != MAPINDEX){
        printf("unmappage:The addr:0x%X isn't a mapped addr!\n",addr);
        return;
    }
    
    uint8 index=getpagec(addr);
    if(!isavl(MTMAP,index)){
       setavl(MTMAP,index);
       TMPMAP[index].base=0;
       TMPMAP[index].P=0;
    }else{
        printf("unmappage:The addr:0x%X hasn't mapped in index:%d!\n",addr,index);
    }
    return;
}


void pagecpy(uint32 dest,uint32 src){
    char *pdest=mappage(dest);
    char *psrc=mappage(src);
    memcpy(pdest,psrc,PAGESIZE);
    
    unmappage(pdest);
    unmappage(psrc);
}


/*
void devpage(uint32 page){
    int i;
    for(i=0;i<256;++i){
        if(PSL[i].pagec==page){
            uint16 next=PSL[i].next;
            while(next!=0){
                KINDEX[TMPINDEX0].base=PSL[next].pagec;
                ptable *pte=getvmaddr(0,TMPINDEX0);
                invlpg(pte);
                if(pte[PSL[next].index].base==page){
                    
                }
            }
            break;
        }
    }
}*/
