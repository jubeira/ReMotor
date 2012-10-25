#include "rtc.h"
#include "iic.h"
#include "timers.h"

#define RTC_ADDRESS 0b01101000
#define RTC_DATA_SIZE (8)
#define RTC_CONTROL_CONFIG(a) (a = 0b00010000)	// 1Hz, output enabled

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
	u8 startUpStage;
	s8 timId;
	rtc_ptr extCB;
} rtc_intData = {_FALSE, 0, INVALID_TIMER, NULL};


void rtc_intSrv (void);
void rtc_setRegAdd (RTC_REG_ADD reg, rtc_ptr cb);
void rtc_storeReceivedData (void);


void rtc_init (bool setTime)
{
	if (rtc_intData.init == _FALSE)
	{
		iic_init();

		tim_init();
		rtc_intData.timId = tim_getTimer(TIM_IC,rtc_intSrv,NULL);

		rtc_intData.extCB = NULL;
		
		rtc_intData.startUpStage = 0;
		rtc_startUp(setTime);
	}
	
	return;
}

void rtc_setTimeAndInit(decimal sec, decimal min, decimal h, decimal date, 
									decimal month, decimal year, rtc_day d, rtc_hourFormat f)
{
	rtc_setInternalTime(decimal sec, decimal min, decimal h, decimal date, 
									decimal month, decimal year, rtc_day d, rtc_hourFormat f);
	
	rtc_init(_TRUE);

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
	// Si falla puedo hacer algo o mando un null y fuck you?
	iic_receive (RTC_ADDRESS, rtc_storeReceivedData, iic_ptr commFailedCB, RTC_DATA_SIZE);
	//hago un read
	//guardo la data
	//cuando termino llamo al callback
	
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
	
	if (rtc_intData.extCB != NULL)
		(*rtc_intData.extCB)();
	
	return;   
}

void rtc_setInternalTime(decimal sec, decimal min, decimal h, decimal date, 
									decimal month, decimal year, rtc_day d, rtc_hourFormat f)
{
	//check data in
	if (sec.deca <= 5 && sec.uni <= 9)
		rtc_data.seconds = sec;
	if (min.deca <= 5 && min.uni <=9)
    	rtc_data.minutes = min;
	if (f == RTC_12_HOUR && (decimal2u8(h) <= 12))
		rtc_data.hours = h;
	else if (f == RTC_24_HOUR && (decimal2u8(h) <= 24))
		rtc_data.hours = h;
	if (decimal2u8(date) <=31)
    	rtc_data.date = date;		// bue bue, días del mes
    if (decimal2u8(month) <= 12)
    	rtc_data.month = month;
    if (decimal2u8(year) <= 99)
    	rtc_data.year = year;
    rtc_data.day = d;
    
    rtc_data.format = f;
    return;
}


u8 decimal2u8(decimal d)
{
	return d.deca*10+d.uni;
}


void rtc_startUp (bool setTime)
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
		if(setTime == _TRUE)
			iic_send(RTC_ADDRESS, rtc_startUp, NULL);
		else
			iic_receive(RTC_ADDRESS,rtc_startUp,NULL, 7); // Ver
			
		rtc_intData.startUpStage++;

		break;

	case 2:
		// Guardo la información recibida, y configuro las settings (sin cambiar el resto de la data)
		// si estaba escribiendo y terminé de escribir, ya estoy listo
		if (setTime == _TRUE)
		{
			rtc_intData.init = _TRUE;
			return;
		}
		rtc_storeReceivedData();
		rtc_setAllRegisters(rtc_startUp);
		rtc_intData.startUpStage++;

		break;

	case 3:
		// Recién ahora está inicializado al dispositivo, se habilitan las interrupciones por el clock del RTC
		rtc_enable();			
		rtc_intData.init = _TRUE;
		break;
	}

	return;
}
