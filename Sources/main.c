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
		if (irData > 0)
			printf("%d\n",(u16)irData);
	}	
}

void init (void)
{
	ir_init();
	
	_asm cli;
}

/* **********************************************************************
 * Funciones de env�o y recepci�n de datos por puerto serie.
 * Los accesos a los streams stdout, stdin, stderr entran y salen por ac�
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
