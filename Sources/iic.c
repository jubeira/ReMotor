#include "iic.h"

#define IIC_FLG_CLEAR() (IIC0_IBSR_IBIF = 1)

typedef void (*f)(void)  iic_ptr;

struct {
    iic_ptr currCB;
    iic_ptr eotCB;
    iic_ptr commFailedCB;
    u8 dataIdx;
}iic_data;

vodi iic_init (void)
{
    IIC0_IBCR_IBEN = 1;
    //falta baud rate
    IIC0_IBSR_IBIF = 1;
    IIC0_IBCR_IBIE = 1;
    
    return;
}

bool iic_send (u8 slvAddress, iic_ptr eotCB)
{
    if (isIICBusy())
        return _FALSE;
    
    iic_data.eotCB = eotCB;
    iic_data.currCB = iic_write;
    iic_data.dataIdx = 0;
    
    IIC0_IBCR_TX_RX = 1;
    IIC0_IBCR_MS_SL = 1;
    IIC0_IBDR = slvAddress << 1 + 1;
    
    return _TRUE;
}

void interrupt iic0_srv (void)
{
    
    if (IIC0_IBSR_RXAK == 1) //received nack
    {
        (*iic_data.commFailedCB)();
        IIC_FLG_CLEAR();
        return;
     }
     
     if (iic_data.currCB == iic_data.eotCB)
        IIC0_IBCR_MS_SL = 0;
    
     (*iic_data.currCB)();
     
    IIC_FLG_CLEAR();   
    
    return;
}

void iic_read (void)
{
}

void iic_write (void)
{
    if (iic_data.dataIdx == iic_commData.dataSize)
        iic_data.currCB = iic_data.eotCB;
    
    IIC0_IBDR = iic_commData.data[iic_data.dataIdx];
    iic_data.dataIdx++;
        
    return;
}