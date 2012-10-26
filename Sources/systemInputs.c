#include "systemInputs.h"

#define COMMAND_PRIVILEGED

static commandData_T commandData = {0, 0, _FALSE};


void step_key2command(u8 key);
void pwm_key2command(u8 key);

u8 filterInt (s16 _byte);
bool isDigit(u8 _byte);
u16 pow10(u8 exp);

bool ir_getCommand(runMode mode)
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
				if ((auxWord = ir_pop()) < 0) // < 0 es un error de pop (nothing to pop)
					break;			// newCommand queda false
				
				filteredByte = filterInt (auxWord);
				
				if (isDigit(auxWord))
				{
					if (commandData.quantity >= 100)
						commandData.quantity %= 100;
					commandData.quantity = commandData.quantity*10 + filteredByte;			// Quantity siempre queda; vuelve a 0 cuando encuentro algo
				}
				else
				{
					step_key2command(auxWord);
					if (commandData.type = NO_COMMAND)
						break;	// newCommand queda false
						
					newCommandFound = _TRUE;
				}
			}
			break;
		case PWM:
			if ((auxWord = ir_pop()) < 0)
				break;						// newCommand queda false.
			
			filteredByte = filterInt (auxWord);
			
			pwm_key2command(filteredByte);
			if (commandData.type = NO_COMMAND)
				break;	// newCommand queda false
			
			newCommandFound = _TRUE;	
			break;
		
		default:
			break;
	}
	
	return newCommandFound;
}


u16 pow10(u8 exp)
{
	if (exp != 0)
		return 10*pow10(exp-1);
	else 
		return 1;
}


void step_key2command(u8 key)
{
	switch (key)
	{	
		case PLAY:
			commandData.quantity = 0;
			commandData.type = RUN;
			break;
		case FAST_FORWARD:
			commandData.type = MOV_FWD;
			break;
		case REWIND:
			commandData.type = MOV_BWD;
			break;
		case STOP_BTN:
			commandData.quantity = 0;
			commandData.type = STOP;
			break;
		case VOL_UP:
			commandData.type = MOV_FWD;
			commandData.quantity = 1;
			break;
		case VOL_DOWN:
			commandData.type = MOV_BWD;
			commandData.quantity = 1;
			break;
		case MUTE:
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
		key = DIGIT;
	
	switch (key)
	{	
		case VOL_UP:
			commandData.type = DUTY_UP;
			break;
		case VOL_DOWN:
			commandData.type = DUTY_DOWN;
			break;
		case MUTE:
			commandData.type = MODE_TOGGLE;
			commandData.quantity = 0;
			commandData.resetFlag = _TRUE;
			break;
		case DIGIT:
			commandData.type = PERIOD_CHANGE;
			commandData.quantity = key;
			break;
		default:
			commandData.type = NO_COMMAND;
			break;
	}
	return;
}

u8 filterInt (s16 _byte)
{
	return ((u8)_byte &= ~(1<<7));
}

bool isDigit(u8 _byte)
{
	if (_byte >= 0 && _byte <= 9)
		return _TRUE;
	else
		return _FALSE;
}
