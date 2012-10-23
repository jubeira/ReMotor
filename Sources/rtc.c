#include "rtc.h"
#include "iic.h"

#define RTC_ADDRESS 0b01101000

void rtc_init (void)
{
	//hablar con el rtc, setearle la configuracion (24 horas, generar cuadrada de 1hz)
	//pedirle la hora
	//guardar la hora
	//activar el input capture para que interrumpa
}

void input capcutre interrupt rtc(void)
{
	iic_commData.dataSize = 7;
	iic_receive(RTC_ADDRESS,functionEOT,NULL);
}

void functionEOT(void)
{
	for i << data.size
		rtc_data[i] = iic_data.data[i]
		
	if rtc_cb != 0
		rtc_cb() //actualiza display
}
        
