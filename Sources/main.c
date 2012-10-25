#include "common.h"
#include "rti.h"
#include "led.h"
#include "display.h"
#include "kbd.h"
#include "ir.h"
#include "iic.h"
#include <stdio.h>
#include "mc9s12xdp512.h"

void init(void);

void main(void)
{
	init();
	for(;;)
	{
		s16 irData = ir_pop();
		if (irData != IR_NO_COMMAND)
			printf("%x\n",(u16)irData);
	}	
}

void init (void)
{
	// Modulos que no requieren interrupciones para inicializar
	ir_init();

	_asm cli;
	// Modulos que sí requieren interrupciones para inicializar	
	rtc_init();
}

/* **********************************************************************
 * Funciones de envÌo y recepciÛn de datos por puerto serie.
 * Los accesos a los streams stdout, stdin, stderr entran y salen por ac·
 * ********************************************************************** */
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
