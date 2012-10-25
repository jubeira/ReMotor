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

#define BCD_HOUR_FORMAT 0x40
#define BCD_HOUR_FORMAT_SHIFT 6

#define RTC_24_HOUR_FORMAT 0
#define RTC_HOUR_FORMAT_SHIFT 6
#define RTC_SQWE_ENABLE 1
#define RTC_SQWE_SHIFT 4
#define RTC_RS0_1HZ 0
#define RTC_RS0_SHIFT 0
#define RTC_RS1_1HZ 0
#define RTC_RS1_SHIFT 1

#define RTC_SEC_REG 0x00
#define RTC_MIN_REG 0x01
#define RTC_HOUR_REG 0x02 
#define RTC_DAY_REG 0x03
#define RTC_DATE_REG 0x04 
#define RTC_MONTH_REG 0x05 
#define RTC_YEAR_REG 0x06 
#define RTC_CONTROL_REG 0x07

rtc_data_T rtc_data;

struct
{
	bool init;
	u8 startUpStage;
	s8 timId;
	rtc_ptr extCB;
} rtc_intData = {_FALSE, 0, INVALID_TIMER, NULL};


void rtc_startUp (void);
void rtc_setRegAdd (u8 reg, rtc_ptr cb);
void rtc_storeReceivedData (void);
void rtc_intSrv (void);
void rtc_intAux (void);

void rtc_init (void)
{
	if (rtc_intData.init == _FALSE)
	{
		iic_init();

		tim_init();
		rtc_intData.timId = tim_getTimer(TIM_IC,rtc_intSrv,NULL);

		rtc_intData.extCB = NULL;
		
		rtc_intData.startUpStage = 0;
		rtc_startUp();
	}
	
	return;
}


void rtc_startUp (void)
{	
	switch (rtc_intData.startUpStage)
	{
	case 0:
		// Preparo la lectura
		rtc_setRegAdd(0,rtc_startUp);
		rtc_intData.startUpStage++;
		
		break;
		
	case 1:
		// Leo los 7 registros que contienen información de hora
		iic_receive(RTC_ADDRESS,rtc_startUp,NULL,6);
		rtc_intData.startUpStage++;
		
		break;
		
	case 3:
		// Guardo la información recibida, y configuro las settings (sin cambiar el resto de la data)
		rtc_storeReceivedData();
		rtc_setAllRegisters(rtc_startUp);
		rtc_intData.startUpStage++;
		
		break;
		
	case 4:
		// Recién ahora está inicializado al dispositivo, se habilitan las interrupciones por el clock del RTC
		tim_enableInterrupts(rtc_intData.timId);
		rtc_enable();			

		break;
	}

	return;
}


void rtc_setRegAdd (u8 reg, rtc_ptr cb)
{
	iic_commData.data[0] = reg;
	iic_commData.dataSize = 0;

	iic_send(RTC_ADDRESS,cb,NULL);
	
	return;
}


void rtc_storeReceivedData (void)
{
	rtc_data.seconds.uni = (iic_commData[RTC_SEC_REG] & 0x10) >> BCD_UNI_SHIFT;
	rtc_data.seconds.deca = (iic_commData[RTC_SEC_REG] & BCD_SEG_DECA) >> BCD_DECA_SHIFT;
	
	rtc_data.minutes.uni = (iic_commData[RTC_MIN_REG] & BCD_UNI) >> BCD_UNI_SHIFT;
	rtc_data.minutes.deca = (iic_commData[RTC_MIN_REG] & BCD_MIN_DECA) >> BCD_DECA_SHIFT;
	
	rtc_data.hours.uni = (iic_commData[RTC_HOUR_REG] & BCD_UNI) >> BCD_UNI_SHIFT;
	rtc_data.hours.deca = (iic_commData[RTC_HOUR_REG] & BCD_HOUR_DECA) >> BCD_DECA_SHIFT; //Se asume formato 24 horas
	
	rtc_data.date.uni = (iic_commData[RTC_DATE_REG] & BCD_UNI) >> BCD_UNI_SHIFT;
	rtc_data.date.deca = (iic_commData[RTC_DATE_REG] & BCD_DATE_DECA) >> BCD_DECA_SHIFT;
	
	rtc_data.day = (iic_commData[RTC_DAY_REG] & BCD_DAY_DECA) >> BCD_DECA_SHIFT;
	
	rtc_data.month.uni = (iic_commData[RTC_MONTH_REG] & BCD_UNI) >> BCD_UNI_SHIFT;
	rtc_data.month.deca = (iic_commData[RTC_MONTH_REG] & BCD_MONTH_DECA) >> BCD_DECA_SHIFT;
	
	rtc_data.year.uni = (iic_commData[RTC_YEAR_REG] & BCD_UNI) >> BCD_UNI_SHIFT;
	rtc_data.year.deca = (iic_commData[RTC_YEAR_REG] & BCD_YEAR_DECA) >> BCD_DECA_SHIFT;
	
	if (rtc_intData.extCB != NULL)
		(*rtc_intData.extCB)();
	
	return;   
}


void rtc_setAllRegisters (rtc_ptr cb)
{
	// Registro inicial de escritura
	iic_commData.data[0] = RTC_SEC_REG;
	iic_commData.data[RTC_SEC_REG] = rtc_data.seconds.deca << BCD_DECA_SHIFT + rtc_data.seconds.uni << BCD_UNI_SHIFT;
	iic_commData.data[RTC_MIN_REG] = rtc_data.minutes.deca << BCD_DECA_SHIFT + rtc_data.minutes.uni << BCD_UNI_SHIFT;
	iic_commData.data[RTC_HOUR_REG] = rtc_data.hours.deca << BCD_DECA_SHIFT + rtc_data.hours.uni << BCD_UNI_SHIFT
										+ RTC_24_HOUR_FORMAT << RTC_HOUR_FORMAT_SHIFT;
	iic_commData.data[RTC_DATE_REG] = rtc_data.date.deca << BCD_DECA_SHIFT + rtc_data.date.uni << BCD_UNI_SHIFT;
	iic_commData.data[RTC_DAY_REG] = rtc_data.day.deca << BCD_DECA_SHIFT + rtc_data.day.uni << BCD_UNI_SHIFT;
	iic_commData.data[RTC_MONTH_REG] = rtc_data.month.deca << BCD_DECA_SHIFT + rtc_data.month.uni << BCD_UNI_SHIFT;
	iic_commData.data[RTC_YEAR_REG] = rtc_data.year.deca << BCD_DECA_SHIFT + rtc_data.year.uni << BCD_UNI_SHIFT;
	iic_commData.data[RTC_CONTROL_REG] = RTC_SQWE_ENABLE << RTC_SQWE_SHIFT + RTC_RS0_1HZ << RTC_RS0_SHIFT + RTC_RS1_1HZ << RTC_RS1_SHIFT;
	
	iic_commData.dataSize = 7;
	
	iic_send(RTC_ADDRESS,cb,NULL);
	
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
	rtc_setRegAdd(0,rtc_intAux);
	
	return;
}

void rtc_intAux (void)
{
	iic_receive(RTC_ADDRESS,rtc_storeReceivedData,NULL,6);
	
	return;
}