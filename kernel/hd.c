#include <hd.h>
#include <common.h>

static uint8 hdstats;

static uint16 nsector;
static uint32 sector;
static uint32 BUS;
static uint32 DRIVE;
static int    CMD;

void HdIntHandler() {
    outp( 0x20, 0x20 );
    outp( 0xa0, 0x20 );
    hdstats = 0xff;
}

static void WaitInit() {
    while ( hdstats == 0 );

    hdstats = 0;
}

static void sendcmd( uint8 cmd ) {
    while ( ( inp( CMD ) & 0xc0 ) != 0x40 );

//    outp(HD_CMD,HdInfo[0].ctl);
//    outp(HD_PRECOMP,HdInfo[0].wpcom>>2);
    outp( BUS + HD_NSECTOR1, nsector >> 8 );        //读写扇区数，高8位
    outp( BUS + HD_LBA3, sector >> 24 );
    outp( BUS + HD_LBA4, 0 );
    outp( BUS + HD_LBA5, 0 );
    outp( BUS + HD_NSECTOR, nsector );              //读写扇区数，低8位
    outp( BUS + HD_LBA0, sector );                  //起始扇区
    outp( BUS + HD_LBA1, sector >> 8 );
    outp( BUS + HD_LBA2, sector >> 16 );
    outp( BUS + HD_EVSEL, 0x40 | DRIVE );
    outp( BUS + HD_COMMAND, cmd );
}

static int readytrans( void ) {
    while ( 1 ) {
        int r = inp( BUS + HD_STATUS );

        if ( r & 0x80 )
            continue;

        if ( r & 0x08 )
            return 0;
    }

    return 1;
}

int Gethdtype( int bus, int drive ) {
    int r;
    outp( bus + HD_EVSEL, drive | 0xa0 );
    outp( bus + HD_NSECTOR, 0 );
    outp( bus + HD_LBA0, 0 );
    outp( bus + HD_LBA1, 0 );
    outp( bus + HD_LBA2, 0 );
    outp( bus + HD_COMMAND, IDENTIFY );
    r = inp( bus + HD_STATUS );

    if ( r == 0 || r == 0xff )
        return 0;
    else {
        while ( inp( bus + HD_STATUS ) & 0x80 );

        unsigned char cl = inp( bus + HD_LCYL ); /* get the "signature bytes" */
        unsigned char ch = inp( bus + HD_HCYL );

        /* differentiate ATA, ATAPI, SATA and SATAPI */
        if ( cl == 0 && ch == 0 )     {
            do {
                r = inp( bus + HD_STATUS );
            } while ( ( ( r & 0x08 ) == 0 ) && ( ( r & 0x01 ) == 0 ) );

            if ( ( r & 0x01 ) == 0 ) {
                unsigned short buff[256];
                int count, counth;
                inpwn( bus + HD_DATA, buff, 256 );

                if ( buff[83] & ( 1 << 10 ) ) {
                    putstring( "The disk suport LBA48!\n" );
                }

                count = ( buff[60] << 16 | buff[61] );

                if ( count ) {
                    printf( "The disk has 0x%8X sectors!\n", count );
                }

                counth = ( buff[102] << 16 ) | buff[103];
                count = ( buff[100] << 16 ) | buff[101];

                if ( counth ) {
                    printf( "The disk has 0x%X%8X sectors!\n", counth, count );
                }
            } else {
                putstring( "An unkown error happened!\n" );
                return ATADEV_UNKNOWN;
            }

            return ATADEV_PATA;
        }

        if ( cl == 0x14 && ch == 0xEB ) return ATADEV_PATAPI;

        if ( cl == 0x69 && ch == 0x96 ) return ATADEV_SATAPI;

        if ( cl == 0x3c && ch == 0xc3 ) return ATADEV_SATA;

        return ATADEV_UNKNOWN;
    }
}

void resetHd( int driver ) {
    hdstats = 0;

    if ( Gethdtype( PRIMARY_BUS, MASTER_DRIVE ) == ATADEV_PATA ) {
        CMD = PR_CMD;
        BUS = PRIMARY_BUS;
        DRIVE = MASTER_DRIVE;
    } else if ( Gethdtype( PRIMARY_BUS, SLAVE_DRIVE ) == ATADEV_PATA ) {
        CMD = PR_CMD;
        BUS = PRIMARY_BUS;
        DRIVE = SLAVE_DRIVE;
    } else if ( Gethdtype( SECONDARY_BUS, MASTER_DRIVE ) == ATADEV_PATA ) {

        CMD = SE_CMD;
        BUS = SECONDARY_BUS;
        DRIVE = MASTER_DRIVE;
    } else if ( Gethdtype( SECONDARY_BUS, SLAVE_DRIVE ) == ATADEV_PATA ) {

        CMD = SE_CMD;
        BUS = SECONDARY_BUS;
        DRIVE = SLAVE_DRIVE;
    } else if ( Gethdtype( THIRDARY_BUS, MASTER_DRIVE ) == ATADEV_PATA ) {

        CMD = TH_CMD;
        BUS = THIRDARY_BUS;
        DRIVE = MASTER_DRIVE;
    } else if ( Gethdtype( THIRDARY_BUS, SLAVE_DRIVE ) == ATADEV_PATA ) {

        CMD = TH_CMD;
        BUS = THIRDARY_BUS;
        DRIVE = SLAVE_DRIVE;
    } else if ( Gethdtype( FORTHARY_BUS, MASTER_DRIVE ) == ATADEV_PATA ) {

        CMD = FO_CMD;
        BUS = FORTHARY_BUS;
        DRIVE = MASTER_DRIVE;
    } else if ( Gethdtype( FORTHARY_BUS, SLAVE_DRIVE ) == ATADEV_PATA ) {

        CMD = FO_CMD;
        BUS = FORTHARY_BUS;
        DRIVE = SLAVE_DRIVE;
    } else {
        putstring( "Can't find a HardDisk!\n" );

        while ( 1 );
    }

    outp( CMD, 4 );
    outp( CMD, 0 );

    while ( ( inp( CMD ) & 0xc0 ) != 0x40 );
}



void readHd( int sec, int n, uint8 *buff ) {
    sector = sec;
    nsector = n;
    sendcmd( LBA48_READ );

    while ( nsector-- ) {
        WaitInit();

        if ( readytrans() ) {
            putstring( "HardDisk Read Error!\n" );
        }

        inpwn( BUS + HD_DATA, buff, 256 );
        buff += 512;
    }

    while ( inp( BUS + HD_STATUS ) & BUSY_STAT );
}

void writeHd( int sec, int nsec, void *buff ) {
}
