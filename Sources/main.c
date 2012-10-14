#include "common.h"
#include "rti/rti.h"
#include "led.h"
#include "graphics.h"

void init();

void main(void)
{
	init();
	
	while(1)
	{	
	}
	
}

void init (void)
{
	rti_init();
	led_init();
	
	_asm cli;
}
