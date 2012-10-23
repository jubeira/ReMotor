#include "common.h"
#include "rti/rti.h"
#include "led.h"
#include "graphics.h"
#include "rc5table.h"
#include "a7table.h"
#include "ir.h"
#include "display.h"
#include "kbd.h"

void init(void);
void test_remote(u8 remote);

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
//	led_init();
	display_init();
//	kb_init();
	ir_init();
	
	_asm cli;
}