#ifndef _RTC_H
#define _RTC_H

#include "common.h"
#include "mc9s12xdp512.h"

#define IIC_MEM_SIZE 256
#define IS_IIC_BUSY() (IIC0_IBSR_IBB ? _TRUE : _FALSE)

typedef void (*iic_ptr)(void);

struct {
    u8 data[IIC_MEM_SIZE];
    u8 dataSize;
} iic_commData;

void iic_init (void);
bool iic_send (u8 slvAddress, iic_ptr eotCB, iic_ptr commFailedCB);
bool iic_receive (u8 slvAddress, iic_ptr eotCB, iic_ptr commFailedCB);

#endif
