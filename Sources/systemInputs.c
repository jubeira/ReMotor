#define _COMMAND_PRIVILEGED

#include "systemInputs.h"
#include "ir.h"
#include "rc5table.h"
#include "kbd.h"

#define KB_SHARP ((s16)(KB_SPECIAL_R - '0'))
#define KB_ASTERISC ((s16)(KB_SPECIAL_L - '0'))

commandData_T commandData = {0, 0, _FALSE};


void step_key2command(u8 key);
void pwm_key2command(u8 key);

u8 filterInt (s16 _byte);

bool in_getCommand(runMode mode)
{
	s16 auxWord;
	u8 filteredByte;
	bool newCommandFound = _FALSE;
	
	if (commandData.resetFlag == _TRUE)
	{
		commandData.quantity = 0;
		commandData.resetFlag = _FALSE;
	}
	
	switch (mode)
	{
		case STEPPER:
			while (!newCommandFound)	
			{
				if ((auxWord = ir_pop()) < 0) // < 0: (nothing to pop)
					break;			// newCommand stays false
				
				filteredByte = filterInt ((u8) auxWord);
				
				if (isDigit(filteredByte))
				{
					if (commandData.quantity >= 100)
						commandData.quantity %= 100;	// Take most significant digit out
					commandData.quantity = commandData.quantity*10 + filteredByte;			
				}
				else
				{
					step_key2command(filteredByte);
					if (commandData.type = NO_COMMAND)
						break;	// newCommand stays false
						
					newCommandFound = _TRUE;
				}
			}
			break;
		case PWM:
			if ((auxWord = ir_pop()) < 0)	// if (pop error)
			{
				if ((auxWord = kb_read()) > 0)	// Check KB as second option
					auxWord -= '0';
				else
					break;		// newCommand stays false.
			}
			filteredByte = filterInt ((u8) auxWord);
			
			pwm_key2command(filteredByte);
			if (commandData.type = NO_COMMAND)
				break;	// newCommand stays false
			
			newCommandFound = _TRUE;	
			break;
		
		default:
			break;
	}
	
	return newCommandFound;
}


void step_key2command(u8 key)
{
	switch (key)
	{	
		case RC5_PLAY:
			//commandData.quantity = 0;	use quantity as speed
			commandData.type = RUN;
			break;
		case RC5_FF:
			commandData.type = MOV_FWD;
			break;
		case RC5_REW:
			commandData.type = MOV_BWD;
			break;
		case RC5_STOP:
			commandData.quantity = 0;
			commandData.type = STOP;
			break;
		case RC5_VOL_UP:
			commandData.type = MOV_FWD;
			commandData.quantity = 1;
			break;
		case RC5_VOL_DOWN:
			commandData.type = MOV_BWD;
			commandData.quantity = 1;
			break;
		case RC5_MUTE:
			commandData.type = MODE_TOGGLE;
			commandData.quantity = 1;		// Default period
			break;
		default:
			commandData.type = NO_COMMAND;
			commandData.quantity = 0;
			break;
	}
	commandData.resetFlag = _TRUE;

}

void pwm_key2command(u8 key)
{
	if (isDigit(key))
		key = RC5_DIGIT;
	
	switch (key)
	{	
		case KB_SHARP: RC5_VOL_UP: 
			commandData.type = DUTY_UP;
			break;
		case KB_ASTERISC: RC5_VOL_DOWN:
			commandData.type = DUTY_DOWN;
			break;
		case RC5_MUTE:
			commandData.type = MODE_TOGGLE;
			commandData.quantity = 0;
			commandData.resetFlag = _TRUE;
			break;
		case RC5_DIGIT:
			commandData.type = PERIOD_CHANGE;
			commandData.quantity = key;
			break;
		default:
			commandData.type = NO_COMMAND;
			break;
	}
	return;
}


u8 filterInt (u8 _byte)
{
	return (_byte &= ~(1<<7));
}
