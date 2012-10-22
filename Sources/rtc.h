#ifndef _RTC_H
#define _RTC_H

typedef void (*f)(void) rtc_ptr;
typedef struct decimal
{
    u8 uni;
    u8 deca;
};

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

void rtc_init (void);

#endif