#include "rtc.h"
#include "iic.h"
#include "timers.h"

#define RTC_ADDRESS 0b01101000

#define BCD_DATE 0x0F
#define BCD_UNI 0x0F
#define BCD_SEG_DECA 0x70
#define BCD_MIN_DECA 0x70
#define BCD_HOUR_DECA 0x30
#define BCD_DATE_DECA 0x30
#define BCD_MONTH_DECA 0x10
#define BCD_YEAR_DECA 0xF0

#define BCD_UNI_SHIFT 0
#define BCD_DECA_SHIFT 4

typedef enum
{
	RTC_SEC_REG,
	RTC_MIN_REG,
 	RTC_HOUR_REG, 
 	RTC_DAY_REG,
 	RTC_DATE_REG, 
 	RTC_MONTH_REG, 
 	RTC_YEAR_REG, 
 	RTC_CONTROL_REG
} RTC_REG_ADD;

rtc_data_T rtc_data;

struct
{
	bool init;
	s8 timId;
	rtc_ptr extCB;
} rtc_intData = {_FALSE, INVALID_TIMER, NULL};


void rtc_intSrv (void);
void rtc_setRegAdd (RTC_REG_ADD reg, rtc_ptr cb);
void rtc_storeReceivedData (void);


void rtc_init (void)
{
	if (rtc_intData.init == _FALSE)
	{
		rtc_intData.init = _TRUE;
		
		iic_init();
		//hablar con el rtc, setearle la configuracion (24 horas, generar cuadrada de 1hz)
		//pedirle la hora
		//guardar la hora
		
		tim_init();
		rtc_intData.timId = tim_getTimer(TIM_IC,rtc_intSrv,NULL);
		
		
		rtc_intData.extCB = NULL;		
	}
	
	return;
}


void rtc_enable (void)
{
	tim_enableInterrupts(rtc_intData.timId);
	return;
}


void rtc_disable (void)
{
	tim_disableInterrupts(rtc_intData.timId);

	return;
}


void rtc_intSrv (void)
{
	//hago un read
	//guardo la data
	//cuando termino llamo al callback
	if (rtc_intData.extCB != NULL)
		(*rtc_intData.extCB)();
	
	return;
}


void rtc_setRegAdd (RTC_REG_ADD reg, rtc_ptr cb)
{
	iic_commData.data[0] = reg;
	iic_commData.dataSize = 0;
	
	iic_send(RTC_ADDRESS,cb,NULL);
	
	return;
}


void rtc_storeReceivedData (void)
{
	rtc_data.seconds.uni = (iic_commData[RTC_SEC_REG] & BCD_UNI) >> BCD_UNI_SHIFT;
	rtc_data.seconds.deca = (iic_commData[RTC_SEC_REG] & BCD_SEG_DECA) >> BCD_DECA_SHIFT;
	
	rtc_data.minutes.uni = (iic_commData[RTC_MIN_REG] & BCD_UNI) >> BCD_UNI_SHIFT;
	rtc_data.minutes.deca = (iic_commData[RTC_MIN_REG] & BCD_MIN_DECA) >> BCD_DECA_SHIFT;
	
	rtc_data.hours.uni = (iic_commData[RTC_HOUR_REG] & BCD_UNI) >> BCD_UNI_SHIFT;
	rtc_data.hours.deca = (iic_commData[RTC_HOUR_REG] & BCD_HOUR_DECA) >> BCD_DECA_SHIFT;
	
	rtc_data.date.uni = (iic_commData[RTC_DATE_REG] & BCD_UNI) >> BCD_UNI_SHIFT;
	rtc_data.date.deca = (iic_commData[RTC_DATE_REG] & BCD_DATE_DECA) >> BCD_DECA_SHIFT;
	
	rtc_data.day = (iic_commData[RTC_DAY_REG] & BCD_DAY_DECA) >> BCD_DECA_SHIFT;
	
	rtc_data.month.uni = (iic_commData[RTC_MONTH_REG] & BCD_UNI) >> BCD_UNI_SHIFT;
	rtc_data.month.deca = (iic_commData[RTC_MONTH_REG] & BCD_MONTH_DECA) >> BCD_DECA_SHIFT;
	
	rtc_data.year.uni = (iic_commData[RTC_YEAR_REG] & BCD_UNI) >> BCD_UNI_SHIFT;
	rtc_data.year.deca = (iic_commData[RTC_YEAR_REG] & BCD_YEAR_DECA) >> BCD_DECA_SHIFT;
	
    
}