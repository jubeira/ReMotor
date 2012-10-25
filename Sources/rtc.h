#ifndef _RTC_H
#define _RTC_H

#include "common.h"

typedef struct 
{
    u8 uni;
    u8 deca;
} decimal;

typedef enum 
{
    RTC_MONDAY,
    RTC_TUESDAY,
    RTC_WEDNESDAY,
    RTC_THURSDAY,
    RTC_FRIDAY,
    RTC_SATURDAY,
    RTC_SUNDAY
} rtc_day;

typedef struct 
{
    decimal seconds;
    decimal minutes;
    decimal hours;
    decimal date;
    decimal month;
    decimal year;
    rtc_day day;
} rtc_data_T;

extern rtc_data_T rtc_data;


typedef void (*rtc_ptr)(void);

void rtc_init (void);
void rtc_enable (void);
void rtc_disable (void);
void rtc_assignCB (rtc_ptr rtc_cb);
void rtc_setAllRegisters(rtc_ptr cb);

u8 decimal2u8(decimal d);

#endif