#ifndef _RTC_H
#define _RTC_H

#include "common.h"

typedef struct 
{
    u8 uni;
    u8 deca;
} decimal;

typedef enum {
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY,
    SUNDAY
} rtc_day;

struct {
    decimal seconds;
    decimal minutes;
    decimal hours;
    decimal date;
    decimal month;
    decimal year;
    rtc_day day;
} rtc_data;


typedef void (*rtc_ptr)(void);

void rtc_init (void);
void rtc_disable (void);
void rtc_enable (void);
void rtc_assignCB (rtc_ptr rtc_cb)

#endif