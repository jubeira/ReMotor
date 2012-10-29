#include "common.h"
#include "rti.h"
#include "led.h"
#include "display.h"
#include "kbd.h"
#include "ir.h"
#include "rtc.h"
#include "systemInputs.h"


void init(void);
void dispTime(void);


#define MAX_SPEED 255
#define MIN_SPEED 0

struct 
{
	runMode mode;
	struct
	{
		u8 speed;
		const u8 periods[5];
	}pwm_data;
	
	struct 
	{
		bool running;
	}step_data;
	
}sys_data = {STEPPER, {0, {0, 50, 100, 150, 250}}, {_FALSE}};


void main(void)
{	
	init();
	
	for(;;)
	{
		stepperLoop();
		pwmLoop();
	}	
}


void stepperLoop(void)
{
	
	if (sysData.mode != STEPPER)
		return;
	
	stepperInit();
	while (sysData.mode == STEPPER)
	{
		if (in_getCommand(sysData.mode) == _FALSE)
				continue;
			
		switch (commandData.type)
		{
			case MOV_FWD: 
				smot_setpoint(commandData.quantity);	// VER si est· prendido
				break;
			
			case MOV_BWD:
				smot_setpoint(-commandData.quantity);
				break;
			
			case STOP: case RUN: 
				smot_speed(commandData.quantity);
				break;
				
			case MODE_TOGGLE:
				sys_data.mode = PWM;
				break;
			
			default:
				break;
		}
	}
}


void pwmLoop(void)
{

	if (sysData.mode != PWM)
		return;
	
	pwmInit();
	
	while (sys_data.mode == PWM)
	{
		if (in_getCommand(sysData.mode) == _FALSE)
			continue;
		
		switch (commandData.type)
		{
			case DUTY_UP:
				if (sys_data.pwm_data.speed < MAX_SPEED) 
					mot_speed(++sys_data.pwm_data.speed);
				break;
			
			case DUTY_DOWN:
				if (sys_data.pwm_data.speed > MIN_SPEED) 
					mot_speed(--sys_data.pwm_data.speed);;
				break;
			
			case PERIOD_CHANGE:
				smot_speed(sys_data.pwm_data.periods[commandData.quantity]);
				break;
				
			case MODE_TOGGLE:
				sys_data.mode = STEPPER;
				break;
			
			default:
				break;
		}
	}
}


void dispTime(void)
{
	disp_ram[0] = rtc_data.minutes.deca+'0';
	disp_ram[1] = rtc_data.minutes.uni+'0';
	disp_att_ram[1].use_dot = 1;
	disp_ram[2] = rtc_data.seconds.deca+'0';
	disp_ram[3] = rtc_data.seconds.uni+'0';
	
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
	rtc_assignAutoUpdateCallback(dispTime);
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
