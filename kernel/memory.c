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

void getmemmap(struct Bootinfo *boot)
{
    memset(charbuff, 0, 80 * 25 * 2); //清屏
    memset(MMAP, 0, MAXPAGE >> 3);
    memset(MTMAP, 0xff, 1024 >> 8);
    memset(PSL, 0, 0x100000);
    if (boot->flags & (1 << 6)) {
        struct memmap *map;
        for (map = boot->mmap_addr;
                (uint32)map < (uint32)boot->mmap_addr + boot->mmap_length;
                map = (struct memmap *)((uint32)map + map->size + sizeof(map->size))) {
            printf("The first size:%d,addr:0x%X%08X,lenth:0x%X%08X,type:%d\n",
                   map->size,
                   map->base_addrh,
                   map->base_addr,
                   map->lengthh,
                   map->length,
                   map->type);
            if (map->type == 1) {
                uint8 *tmpaddr = upto(map->base_addr, PAGESIZE);
                map->length -= (tmpaddr - map->base_addr);
                map->base_addr = tmpaddr;
                size_t count = map->length / PAGESIZE;
                size_t startp = (int)map->base_addr / PAGESIZE;
                int i;
                for (i = 0; i < count; ++i) {
                    setavl(MMAP, startp + i);
                }

            }
        }
    } else {
        putstring("Can't get memory map!\n");
    }
    memset(MMAP, 0x00, RESPAGE >> 3);
}

int getmpage()
{
    int i;
    for (i = RESPAGE; i < MAXPAGE; ++i) {
        if (isavl(MMAP, i)) {
            unavl(MMAP, i);
            return i;
        }
    }
    printf("getmpage:Can't get more memory!\n");
    return -1;
}

void freempage(uint32 page)
{
    if (!isavl(MMAP, page)) {
        setavl(MMAP, page);
    } else {
        printf("freempage:The page hasn't used!\n");
    }
    return;
}


void *mappage(uint32 page)
{
    int i;
    for (i = 0; i < 1024; ++i) {
        if (isavl(MTMAP, i)) {
            unavl(MTMAP, i);
            break;
        }
    }
    TMPMAP[i].base = page;
    TMPMAP[i].P = 1;
    void *addr = getvmaddr(MAPINDEX, i);
    invlpg(addr);
    return addr;
}

void unmappage(void *addr)
{
    if (getpagec(addr) != MAPINDEX) {
        printf("unmappage:The addr:0x%X isn't a mapped addr!\n", addr);
        return;
    }

    uint8 index = getpagei(addr);
    if (!isavl(MTMAP, index)) {
        setavl(MTMAP, index);
        TMPMAP[index].base = 0;
        TMPMAP[index].P = 0;
    } else {
        printf("unmappage:The addr:0x%X hasn't mapped in index:%d!\n", addr, index);
    }
    return;
}


void pagecpy(uint32 dest, uint32 src)
{
    char *pdest = mappage(dest);
    char *psrc = mappage(src);
    memcpy(pdest, psrc, PAGESIZE);

    unmappage(pdest);
    unmappage(psrc);
}

void sharepage(uint32 pageca, uint16 pageia, uint32 pagecb, uint16 pageib)
{
    ptable *pagea = mappage(pageca);
    ptable *pageb = mappage(pagecb);
    int i, j;
    if (pagea[pageia].AVL == 1) {
        for (i = 1; PSL[i].pagec != pagea[pageia].base && i < MAX_SHRLC; ++i);
        PSL[i].index++;
        for (j = PSL[i].next; PSL[j].next != 0; j = PSL[j].next);
        for (i = 1; PSL[i].pagec != 0 && i < MAX_SHRLC; ++i);
        PSL[j].next = i;
        PSL[i].pagec = pagecb;
        PSL[i].index = pageib;
        PSL[i].next = 0;
    } else {
        for (i = 1; PSL[i].pagec != 0 && i < MAX_SHRLC; ++i);
        PSL[i].pagec = pagea[pageia].base;
        PSL[i].index = 2;
        for (j = i+1; PSL[j].pagec != 0 && j < MAX_SHRLC; ++j);
        PSL[i].next = j;
        PSL[j].pagec = pageca;
        PSL[j].index = pageia;
        for (i = j+1; PSL[i].pagec != 0 && i < MAX_SHRLC; ++i);
        PSL[j].next = i;
        PSL[i].pagec = pagecb;
        PSL[i].index = pageib;
        PSL[i].next = 0;

    }
    pagea[pageia].AVL = 1;
    pagea[pageia].R_W = 0;
    pageb[pageib].AVL = 1;
    pageb[pageib].R_W = 0;

    unmappage(pagea);
    unmappage(pageb);
}


void devpage(uint32 pagec, uint16 pagei)
{
    ptable *paged = mappage(pagec);
    int i, j;
    for (i = 1; PSL[i].pagec != paged[pagei].base && i < MAX_SHRLC; ++i);
    if (PSL[i].index == 2) {
        j = PSL[i].next;
        ptable *pagef = NULL;
        if (PSL[j].pagec == pagec) {
            pagef = mappage(PSL[PSL[j].next].pagec);
            pagef[PSL[PSL[j].next].index].AVL = 0;
            pagef[PSL[PSL[j].next].index].R_W = 1;

        } else {
            pagef = mappage(PSL[j].pagec);
            pagef[PSL[j].index].AVL = 0;
            pagef[PSL[j].index].R_W = 1;
        }
        unmappage(pagef);
        
        PSL[PSL[j].next].pagec = 0;
        PSL[j].pagec = 0;
        PSL[i].pagec = 0;
    } else {
        PSL[i].index--;
        for (j = i; PSL[PSL[j].next].pagec != pagec ; j = PSL[j].next);
        PSL[PSL[j].next].pagec=0;
        PSL[j].next=PSL[PSL[j].next].next;
    }
    
    paged[pagei].AVL=0;
    paged[pagei].R_W=1;
    unmappage(paged);
}
