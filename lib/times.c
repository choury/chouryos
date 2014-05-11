#include <common.h>
#include <sys/time.h>


#define CURRENT_YEAR        2014                            // Change this each year!

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


/*
 * Copyright (C) 1993, 1994, 1995, 1996, 1997 Free Software Foundation, Inc.
 * This file is part of the GNU C Library.
 * Contributed by Paul Eggert (eggert@twinsun.com).
 *
 * The GNU C Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * The GNU C Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with the GNU C Library; see the file COPYING.LIB.  If not,
 * write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Converts the calendar time to broken-down time representation
 * Based on code from glibc-2.6
 *
 * 2009-7-14:
 *   Moved from glibc-2.6 to kernel by Zhaolei<zhaolei@cn.fujitsu.com>
 */


/*
 * Nonzero if YEAR is a leap year (every 4 years,
 * except every 100th isn't, and every 400th is).
 */
static int __isleap(long year)
{
    return (year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0);
}

/* do a mathdiv for long type */
static long math_div(long a, long b)
{
    return a / b - (a % b < 0);
}

/* How many leap years between y1 and y2, y1 must less or equal to y2 */
static long leaps_between(long y1, long y2)
{
    long leaps1 = math_div(y1 - 1, 4) - math_div(y1 - 1, 100)
        + math_div(y1 - 1, 400);
    long leaps2 = math_div(y2 - 1, 4) - math_div(y2 - 1, 100)
        + math_div(y2 - 1, 400);
    return leaps2 - leaps1;
}

/* How many days come before each month (0-12). */
static const unsigned short __mon_yday[2][13] = {
    /* Normal years. */
    {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365},
    /* Leap years. */
    {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366}
};


#define SECS_PER_HOUR   (60 * 60)
#define SECS_PER_DAY    (SECS_PER_HOUR * 24)


/**
 * time_to_tm - converts the calendar time to local broken-down time
 *
 * @totalsecs   the number of seconds elapsed since 00:00:00 on January 1, 1970,
 *      Coordinated Universal Time (UTC).
 * @offset  offset seconds adding to totalsecs.
 * @result  pointer to struct tm variable to receive broken-down time
 */
void time_to_tm(time_t totalsecs, struct tm *result)
{
    long days, rem, y;
    const unsigned short *ip;

    days = totalsecs / SECS_PER_DAY;
    rem = totalsecs % SECS_PER_DAY;
    while (rem < 0) {
        rem += SECS_PER_DAY;
        --days;
    }
    while (rem >= SECS_PER_DAY) {
        rem -= SECS_PER_DAY;
        ++days;
    }

    result->tm_hour = rem / SECS_PER_HOUR;
    rem %= SECS_PER_HOUR;
    result->tm_min = rem / 60;
    result->tm_sec = rem % 60;

    /* January 1, 1970 was a Thursday. */
    result->tm_wday = (4 + days) % 7;
    if (result->tm_wday < 0)
        result->tm_wday += 7;

    y = 1970;

    while (days < 0 || days >= (__isleap(y) ? 366 : 365)) {
        /* Guess a corrected year, assuming 365 days per year. */
        long yg = y + math_div(days, 365);

        /* Adjust DAYS and Y to match the guessed year. */
        days -= (yg - y) * 365 + leaps_between(y, yg);
        y = yg;
    }

    result->tm_year = y - 1900;

    result->tm_yday = days;

    ip = __mon_yday[__isleap(y)];
    for (y = 11; days < ip[y]; y--)
        continue;
    days -= ip[y];

    result->tm_mon = y;
    result->tm_mday = days + 1;
}
