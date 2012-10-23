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
