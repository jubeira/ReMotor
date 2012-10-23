#include "common.h"
#include "rti.h"
#include "led.h"
#include "display.h"
#include "kbd.h"
#include "ir.h"
#include "iic.h"


void init(void);

void main(void)
{
	init();
	for(;;)
	{
	}	
}

void init (void)
{
	rti_init();
	led_init();
	display_init();
	kb_init();
	ir_init();
	iic_init();
	
	_asm cli;
}