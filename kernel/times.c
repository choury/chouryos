#include <chouryos.h>
#include <syscall.h>
#include <sys/time.h>

#define CURRENT_YEAR        2013                            // Change this each year!

static int century_register = 0x00;                                // Set by ACPI table parsing code if possible

static unsigned char Second;
static unsigned char Minute;
static unsigned char Hour;
static unsigned char Day;
static unsigned char Month;
static unsigned int  Year;


enum {
    cmos_address = 0x70,
    cmos_data    = 0x71
};


int get_update_in_progress_flag() {
    outp(cmos_address, 0x0A);
    return (inp(cmos_data) & 0x80);
}


unsigned char get_RTC_register(int reg) {
    outp(cmos_address, reg);
    return inp(cmos_data);
}


void read_rtc() {
    unsigned char century;
    unsigned char last_second;
    unsigned char last_minute;
    unsigned char last_hour;
    unsigned char last_day;
    unsigned char last_month;
    unsigned char last_year;
    unsigned char last_century;
    unsigned char registerB;

    // Note: This uses the "read registers until you get the same values twice in a row" technique
    //       to avoid getting dodgy/inconsistent values due to RTC updates

    while (get_update_in_progress_flag());                // Make sure an update isn't in progress
    Second = get_RTC_register(0x00);
    Minute = get_RTC_register(0x02);
    Hour = get_RTC_register(0x04);
    Day = get_RTC_register(0x07);
    Month = get_RTC_register(0x08);
    Year = get_RTC_register(0x09);
    if(century_register != 0) {
        century = get_RTC_register(century_register);
    }

    do {
        last_second = Second;
        last_minute = Minute;
        last_hour = Hour;
        last_day = Day;
        last_month = Month;
        last_year = Year;
        last_century = century;

        while (get_update_in_progress_flag());           // Make sure an update isn't in progress
        Second = get_RTC_register(0x00);
        Minute = get_RTC_register(0x02);
        Hour = get_RTC_register(0x04);
        Day = get_RTC_register(0x07);
        Month = get_RTC_register(0x08);
        Year = get_RTC_register(0x09);
        if(century_register != 0) {
            century = get_RTC_register(century_register);
        }
    } while( (last_second != Second) || (last_minute != Minute) || (last_hour != Hour) ||
             (last_day != Day) || (last_month != Month) || (last_year != Year) ||
             (last_century != century) );

    registerB = get_RTC_register(0x0B);

    // Convert BCD to binary values if necessary

    if (!(registerB & 0x04)) {
        Second = (Second & 0x0F) + ((Second / 16) * 10);
        Minute = (Minute & 0x0F) + ((Minute / 16) * 10);
        Hour = ( (Hour & 0x0F) + (((Hour & 0x70) / 16) * 10) ) | (Hour & 0x80);
        Day = (Day & 0x0F) + ((Day / 16) * 10);
        Month = (Month & 0x0F) + ((Month / 16) * 10);
        Year = (Year & 0x0F) + ((Year / 16) * 10);
        if(century_register != 0) {
            century = (century & 0x0F) + ((century / 16) * 10);
        }
    }

    // Convert 12 hour clock to 24 hour clock if necessary

    if (!(registerB & 0x02) && (Hour & 0x80)) {
        Hour = ((Hour & 0x7F) + 12) % 24;
    }

    // Calculate the full (4-digit) year

    if(century_register != 0) {
        Year += century * 100;
    } else {
        Year += (CURRENT_YEAR / 100) * 100;
        if(Year < CURRENT_YEAR) Year += 100;
    }
}

#if 0

#define MINUTE 60
#define HOUR (60*MINUTE)
#define DAY (24*HOUR)
#define YEAR (365*DAY)

/* interestingly, we assume leap-years */
static int months[12] = {
    0,
    DAY*(31),
    DAY*(31+29),
    DAY*(31+29+31),
    DAY*(31+29+31+30),
    DAY*(31+29+31+30+31),
    DAY*(31+29+31+30+31+30),
    DAY*(31+29+31+30+31+30+31),
    DAY*(31+29+31+30+31+30+31+31),
    DAY*(31+29+31+30+31+30+31+31+30),
    DAY*(31+29+31+30+31+30+31+31+30+31),
    DAY*(31+29+31+30+31+30+31+31+30+31+30)
};


long kernel_mktime(void)
{
    read_rtc();
    long res;
    int year;

    year = Year - 1970;
/* magic offsets (y+1) needed to get leapyears right.*/
    res = YEAR*year + DAY*((year+1)/4);
    res += months[Month-1];
/* and (y+2) here. If it wasn't a leap-year, we have to adjust */
    if (Month>2 && ((year+2)%4))
        res -= DAY;
    res += DAY*(Day-1);
    res += HOUR*(Hour-8);
    res += MINUTE*Minute;
    res += Second;
    return res;
}

#endif

time_t kernel_mktime (
    unsigned int year, unsigned int mon,
    unsigned int day, unsigned int hour,
    unsigned int min, unsigned int sec)
        {
    if (0 >= (int) (mon -= 2)) {    /* 1..12 -> 11,12,1..10 */
         mon += 12;      /* Puts Feb last since it has leap day */
         year -= 1;
    }

    return (((
             (time_t) (year/4 - year/100 + year/400 + 367*mon/12 + day) +
             year*365 - 719499
          )*24 + hour /* now have hours */
       )*60 + min /* now have minutes */
    )*60 + sec; /* finally seconds */
}

time_t kernel_getnowtime(){
    read_rtc();
    return kernel_mktime(Year,Month,Day,Hour,Minute,Second);
}

int sys_gettimeofday(struct timeval *tv, struct timezone *tz){
    tv->tv_sec=kernel_getnowtime();
    tv->tv_usec=0;
    return 0;
}
