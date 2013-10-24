#ifndef __FLOPPY_H__
#define __FLOPPY_H__




#define STATUS_REG_A            0x03f0 /*PS2 SYSTEMS*/
#define STATUS_REG_B            0x03f1 /*PS2 SYSTEMS*/
#define DIGITAL_OUTPUT_REG      0x03f2
#define MAIN_STATUS_REG         0x03f4
#define DATA_RATE_SELECT_REG    0x03f4 /*PS2 SYSTEMS*/
#define DATA_REGISTER           0x03f5
#define DIGITAL_INPUT_REG       0x03f7 /*AT SYSTEMS*/
#define CONFIG_CONTROL_REG      0x03f7 /*AT SYSTEMS*/





/*controller commands*/
#define FIX_DRIVE_DATA          0x03
#define CHECK_DRIVE_STATUS      0x04
#define CALIBRATE_DRIVE         0x07
#define CHECK_INTERRUPT_STATUS  0x08
#define FORMAT_TRACK            0x4D
#define READ_SECTOR             0xC6
#define READ_DELETE_SECTOR      0xCC
#define READ_SECTOR_ID          0x4A
#define READ_TRACK              0x42
#define SEEK_TRACK              0x0F
#define WRITE_SECTOR            0xC5
#define WRITE_DELETE_SECTOR     0xC9

/* Bits of main status register */
#define STATUS_BUSYMASK 0x0F        /* drive busy mask */
#define STATUS_BUSY 0x10        /* FDC busy */
#define STATUS_DMA  0x20        /* 0- DMA mode */
#define STATUS_DIR  0x40        /* 0- cpu->fdc */
#define STATUS_READY    0x80        /* Data reg ready */




void FloppyIntHandler();

void reset_floppy_controller(char drive);
void calibrate_drive(char drive);
void configure_drive(char drive) ;
void send_byte(unsigned char command) ;
unsigned char get_byte();
void seek_track(unsigned char head,unsigned char track,unsigned char drive);
void rw_sector(unsigned char drive,unsigned int block,unsigned char * buffer,unsigned char command);
void readfloppyA(unsigned int block,void *buffer);
void writefloppyA(unsigned int block,void *buffer);




#endif
