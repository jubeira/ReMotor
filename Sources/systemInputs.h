#ifndef _SYSTEM_INPUTS_H
#define _SYSTEM_INPUTS_H

#include "common.h"

#define RC5_DIGIT 200

typedef enum
{
	MOV_FWD, MOV_BWD, RUN, STOP,
	NO_COMMAND, MODE_TOGGLE,
	DUTY_UP, DUTY_DOWN, PERIOD_CHANGE
}command;

typedef enum
{
	STEPPER,
	PWM,
}runMode;

typedef struct 
{
	command type;
	u16 quantity;
	
	bool resetFlag;

}commandData_T;


#ifdef _COMMAND_PRIVILEGED
extern commandData_T commandData;
#else
extern const commandData_T commandData;
#endif


bool in_getCommand(runMode mode);


#endif //_SYSTEM_INPUTS_H