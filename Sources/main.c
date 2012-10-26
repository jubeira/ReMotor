#include "common.h"
#include "rti.h"
#include "led.h"
#include "display.h"
#include "kbd.h"
#include "ir.h"
#include "rtc.h"


void init(void);
void dispTime(void);

void main(void)
{	
	init();
	dispTime();
	led(255);
	rtc_assignAutoUpdateCallback(dispTime);
	
	for(;;)
	{
		
	}	
}

void dispTime(void)
{
	disp_ram[0] = rtc_data.minutes.deca+'0';
	disp_ram[1] = rtc_data.minutes.uni+'0';
	disp_ram[2] = rtc_data.seconds.deca+'0';
	disp_ram[3] = rtc_data.seconds.uni+'0';
	disp_att_ram[1].use_dot = 1;
	
	return;
}


void init (void)
{
	// Modulos que no requieren interrupciones para inicializar
	rti_init();
	led_init();
	display_init();

	//kb_init();
	//ir_init();

	_asm cli;
	// Modulos que si requieren interrupciones para inicializar	
	rtc_init();
}


/* **********************************************************************
 * Funciones de envÃo y recepci€n de datos por puerto serie.
 * Los accesos a los streams stdout, stdin, stderr entran y salen por ac∑
 * ********************************************************************** */
#include "mc9s12xdp512.h"
 
void TERMIO_PutChar(char ch) 
{
    while (!(SCI0SR1 & SCI0SR1_TDRE_MASK));
    SCI0DRL = ch;
}
char TERMIO_GetChar(void) 
{
    while (!(SCI0SR1 & SCI0SR1_RDRF_MASK));
    return SCI0DRL;
}
