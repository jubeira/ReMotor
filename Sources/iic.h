#ifndef _RTC_H
#define _RTC_H

#define IIC_MEM_SIZE 256

struct {
    u8 data[IIC_MEM_SIZE];
    u8 dataSize;
} iic_commData;

bool isIICBusy (void);

#endif