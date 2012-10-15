#include "common.h"
#include "rti/rti.h"
#include "led.h"
#include "graphics.h"
#include "rc5table.h"
#include "a7table.h"
#include "ir.h"

void init(void);
void test_remote(u8 remote);
void main(void)
{
	init();
	
	while(1)
	{
	    s16 remote;
	    while ((remote = irPop()) < 0)
	        ;
            test_remote((u8)remote);
	}
	
}

void init (void)
{
	rti_init();
	led_init();
	
	_asm cli;
}

void test_remote(u8 remote)
{
    switch(remote)
    {
        case RC5_0: case RC5_1: case RC5_2: case RC5_3: case RC5_4: case RC5_5: case RC5_6: case RC5_7: case RC5_8: case RC5_9:
            disp_ram[0] = SPACE;
            disp_ram[1] = SPACE;
            disp_ram[2] = SPACE;
            disp_ram[3] = ascii_to_7[remote+'0'];
            break;
        case RC5_VOL_UP:
            fancy_msg("Vol1");
            break;
        case RC5_VOL_DOWN:
            fancy_msg("Vol2");
            break;
        case RC5_CHANN_UP:
            fancy_msg("chn1");
            break;
        case RC5_CHANN_DOWN:
            fancy_msg("chn2");
            break;
        case RC5_REW:
            fancy_msg("rew");
            break;
        case RC5_FF:
            fancy_msg("ff");
            break;
        case RC5_PLAY:
            fancy_msg("play");
            break;
        case RC5_STOP:
            fancy_msg("stop");
            break;
        default:
            fancy_msg("erro");
            break;
        
    }   
}