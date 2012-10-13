#include "common.h"
#include "rti/rti.h"
#include "console.h"
#include "led.h"
#include "graphics.h"
#include "app/account.h"

#define INPUT_TIMEOUT RTI_MS2PERIOD(3000)
#define STANDBY_LED() led_fade(RTI_MS2PERIOD(100), LED_NON_STOP, 0)

void init();

void main(void)
{
	init();
	
	STANDBY_LED();
	
	while(1)
	{
		char id[ID_LEN];
		char pw[PW_LEN];
		
		con_clear();
		con_input_enable(1);
		con_output_mode(CON_NORMAL);
		
		if (con_gets_timed(id,INPUT_TIMEOUT) == CON_TIMEOUT)
			continue;
	
		con_clear();
		con_input_enable(1);
		con_output_mode(CON_PASSWD);
				
		if (con_gets_timed(pw,INPUT_TIMEOUT) == CON_TIMEOUT) 
			continue;
	
		if (is_valid_account(id,pw))
		{
			led_blink(LED_ON, LED_OFF, RTI_MS2PERIOD(300), RTI_MS2PERIOD(300), RTI_MS2PERIOD(6000), LED_OFF);
			fancy_msg("Pass");
			STANDBY_LED();
		}
		else
			fancy_msg("Error");			
	}
	
}
void led_blink (u8 dim_on, u8 dim_off, rti_time t_on, rti_time t_off, rti_time dur, int end_dim);
void init (void)
{
	rti_init();
	led_init();
	con_init();
	
	_asm cli;
}