#include <type.h>

#ifndef __HD_H__
#define __HD_H__

/* Hd controller regs. Ref: IBM AT Bios-listing */
#define HD_DATA     0x0   /* _CTL when writing */
#define HD_ERROR    0x1   /* see err-bits */
#define HD_NSECTOR  0x2   /* nr of sectors to read/write */
#define HD_NSECTOR1 0x2   /* high 8 bit of sectors to read/write */
#define HD_SECTOR   0x3   /* starting sector */
#define HD_LBA0     0x3   /* LBA0 Register */
#define HD_LBA3     0x3   /* LBA3 Register */
#define HD_LCYL     0x4   /* starting cylinder */
#define HD_LBA1     0x4   /* LBA1 Register */
#define HD_LBA4     0x4   /* LBA4 Register */
#define HD_HCYL     0x5   /* high byte of starting cyl */
#define HD_LBA2     0x5   /* LBA2 Register */
#define HD_LBA5     0x5   /* LBA5 Register */
#define HD_CURRENT  0x6   /* 101dhhhh , d=drive, hhhh=head */
#define HD_EVSEL    0x6   /* 111dhhhh , d=drive, hhhh=LBA4 */
#define HD_STATUS   0x7   /* see status-bits */
#define HD_PRECOMP HD_ERROR /* same io address, read=error, write=precomp */
#define HD_COMMAND HD_STATUS    /* same io address, read=status, write=cmd */

#define PR_CMD      0x3f6
#define SE_CMD      0x376
#define TH_CMD      0x3E6
#define FO_CMD      0x366

#define MASTER_DRIVE  0x00    //master drive
#define SLAVE_DRIVE   0x10

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

#define LBA28_READ      0x20
#define LBA28_WRITE     0x30

#define LBA48_READ      0x24
#define LBA48_WRITE     0x34

#define IDENTIFY        0xec

#define PRIMARY_BUS         0x1F0
#define SECONDARY_BUS       0x170
#define THIRDARY_BUS        0x1E8
#define FORTHARY_BUS        0x168


#define ATADEV_PATAPI       1
#define ATADEV_SATAPI       2
#define ATADEV_PATA         3
#define ATADEV_SATA         4
#define ATADEV_UNKNOWN      5

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

#define HdInfo  ((Hdinfo *)0x1300)      //已废弃

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
