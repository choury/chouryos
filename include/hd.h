#include <type.h>

#ifndef __HD_H__
#define __HD_H__

/* Hd controller regs. Ref: IBM AT Bios-listing */
#define HD_DATA     0x1f0   /* _CTL when writing */
#define HD_ERROR    0x1f1   /* see err-bits */
#define HD_NSECTOR  0x1f2   /* nr of sectors to read/write */
#define HD_NSECTOR1 0x1f2   /* high 8 bit of sectors to read/write */
#define HD_SECTOR   0x1f3   /* starting sector */
#define HD_LBA0     0x1f3   /* LBA0 Register */
#define HD_LBA3     0x1f3   /* LBA3 Register */
#define HD_LCYL     0x1f4   /* starting cylinder */
#define HD_LBA1     0x1f4   /* LBA1 Register */
#define HD_LBA4     0x1f4   /* LBA4 Register */
#define HD_HCYL     0x1f5   /* high byte of starting cyl */
#define HD_LBA2     0x1f5   /* LBA2 Register */
#define HD_LBA5     0x1f5   /* LBA5 Register */
#define HD_CURRENT  0x1f6   /* 101dhhhh , d=drive, hhhh=head */
#define HD_EVSEL    0x1f6   /* 111dhhhh , d=drive, hhhh=LBA4 */
#define HD_STATUS   0x1f7   /* see status-bits */
#define HD_PRECOMP HD_ERROR /* same io address, read=error, write=precomp */
#define HD_COMMAND HD_STATUS    /* same io address, read=status, write=cmd */

#define HD_CMD      0x3f6

/* Bits of HD_STATUS */
#define ERR_STAT    0x01
#define INDEX_STAT  0x02
#define ECC_STAT    0x04    /* Corrected error */
#define DRQ_STAT    0x08
#define SEEK_STAT   0x10
#define WRERR_STAT  0x20
#define READY_STAT  0x40
#define BUSY_STAT   0x80

/* Values for HD_COMMAND */
#define WIN_RESTORE     0x10
#define WIN_READ        0x20
#define WIN_WRITE       0x30
#define WIN_VERIFY      0x40
#define WIN_FORMAT      0x50
#define WIN_INIT        0x60
#define WIN_SEEK        0x70
#define WIN_DIAGNOSE    0x90
#define WIN_SPECIFY     0x91


typedef struct{
    uint16  cyl;
    uint8   heads;
    uint16  unknow;
    uint16  wpcom;
    uint8   unknow1;
    uint8   ctl;
    uint8   unknow2[3];
    uint16  lzone;
    uint8   spt;
    uint8   reserved;
}__attribute__ ((packed)) Hdinfo;

#define HdInfo  ((Hdinfo *)0x1300)

typedef struct{
    uint8 boot_ind;
    uint8 head;
    uint8 sec:6;
    uint8 cylh:2;
    uint8 cyll;
    uint8 sys_ind;
    uint8 end_head;
    uint8 end_sec:6;
    uint8 end_cylh:2;
    uint8 end_cyll;
    uint32 sector;
    uint32 nsector;
}__attribute__ ((packed))HPT;

typedef struct{
    uint8 code[446];
    HPT   hpt[4];
    uint8 sig[2];
}__attribute__ ((packed))MBR;

void HdIntHandler();
void resetHd(int driver);
void readHd(int sec,int n,void *buff);
void writeHd(int sec,int nsec,void *buff);

#endif
