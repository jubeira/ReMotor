#include "mc9s12xdp512.h"
#include "timers.h"
#include "common.h"

#define	TIMER_PRESCALER 2			// [0,7]
#define TIMER_START 1					// {0,1}
#define TIMER_OVERFLOW_INT 1	// {0,1}
#define TIMER_RESET_ENABLE 0	// {0,1}
#define FLAG_AUTOCLR 1				// {0,1}
#define OVF_FLAG_CLR() (TFLG2=0x80)


#define OC_FLAG_CLR() (TFLG1=0x01)
#define OC_CHANNEL (1<<0)				// Mask - must not use IC_CHANNEL bits
#define OC_ACTION1 0x00				// Mask
#define OC_ACTION2 0x00				// Mask

#define OC0_DISCONNECTED {TCTL2_OL0=0;TCTL2_OM0=0;}

#define IC_ACTION1 0x00
#define IC_ACTION2 0x08
#define IC_CHANNEL 1<<1

#define TIM_AMOUNT 8
#define IS_VALID_ID(id) (((id >= 0) && (id < TIM_AMOUNT)) ? _TRUE : _FALSE)

struct {
	bool init;
	bool isTimerUsed[8];
	tim_ptr cbArray[8];
	tim_ptr ovfArray[8];
} tim_data;


void timer_init(void) 
{
	if (tim_data.init == _FALSE)
	{
		u8 i;

		tim_data.init = true;
		
		for (i = 0; i < TIM_AMOUT; i++)
		{
			tim_data.isTimerUsed[i] = _FALSE;
			tim_data.cbArray[i] = NULL;		
			tim_data.ovfArray[i] = NULL;			
		}
		
		//deshabilitar todas las interrupciones
		TSCR2 = 0x00;
		TSCR2_TOI = TIMER_OVERFLOW_INT;
		TSCR2_TCRE = TIMER_RESET_ENABLE;
		TSCR2 |= TIMER_PRESCALER;	//Prescaler: 4 - de 0000 a FFFF: tarda 10ms en overflow
		TSCR1_TEN = TIMER_START; // TEN
	}
	
	return;
}


s8 timId tim_getTimer(tim_type reqType, tim_ptr cb, tim_ptr ovf)
{
	s8 i;
	for (i = 0; i < TIM_AMOUT; i++)
		if (tim_data.isTimerUsed[i] == _FALSE)
		{
			tim_data.isTimerUsed[i] = _TRUE;
			tim_data.cbArray[i] = cb;
			tim_data.ovfArray[i] = ovf;
			//setear si es oc o ic
			//deshabilitar interrupciones
			break;
		}
		
	if (i == TIM_AMOUT)
		i = INVALID_TIMER;
	
	return i;	
}


void tim_freeTimer(s8 timId)
{
	if (!IS_VALID_ID(timdID))
		return;
	
	tim_data.isTimerUsed[i] = _FALSE;
	tim_data.cbArray[i] = NULL;
	tim_data.ovfArray[i] = NULL;
	//deshabilitar interrupciones
	
	return;
}


void tim_setFallingEdge(s8 timId)
{
	if(!IS_VALID_ID(timId))
		return;
	
	switch (timId)
	{
		case 0:
			TCTL4_EDG0A = 0;
			TCTL4_EDG0B = 1;
			break;
		case 1:
			TCTL4_EDG1A = 0;
			TCTL4_EDG1B = 1;
			break;
		case 2:
			TCTL4_EDG2A = 0;
			TCTL4_EDG2B = 1;
			break;
		case 3:
			TCTL4_EDG3A = 0;
			TCTL4_EDG3B = 1;
			break;
		case 4:
			TCTL3_EDG4A = 0;
			TCTL3_EDG4B = 1;
			break;
		case 5:
			TCTL3_EDG5A = 0;
			TCTL3_EDG5B = 1;
			break;
		case 6:
			TCTL3_EDG6A = 0;
			TCTL3_EDG6B = 1;
			break;
		case 7:
			TCTL3_EDG7A = 0;
			TCTL3_EDG7B = 1;
			break;
	}
	
	return;
}


void tim_setRisingEdge(s8 timId)
{
	if(!IS_VALID_ID(timId))
		return;
	
	switch (timId)
	{
		case 0:
			TCTL4_EDG0A = 1;
			TCTL4_EDG0B = 0;
			break;
		case 1:
			TCTL4_EDG1A = 1;
			TCTL4_EDG1B = 0;
			break;
		case 2:
			TCTL4_EDG2A = 1;
			TCTL4_EDG2B = 0;
			break;
		case 3:
			TCTL4_EDG3A = 1;
			TCTL4_EDG3B = 0;
			break;
		case 4:
			TCTL3_EDG4A = 1;
			TCTL3_EDG4B = 0;
			break;
		case 5:
			TCTL3_EDG5A = 1;
			TCTL3_EDG5B = 0;
			break;
		case 6:
			TCTL3_EDG6A = 1;
			TCTL3_EDG6B = 0;
			break;
		case 7:
			TCTL3_EDG7A = 1;
			TCTL3_EDG7B = 0;
			break;
	}
	
	return;
}

void tim_setBothEdge(s8 timId);

void tim_enableInterrupts(s8 timId)
{
	if(!IS_VALID_ID(timId)
		return;
	
	switch (timId)
	{
		case 0:
			TIE_C0I = 1;
			break;
		case 1:
			TIE_C1I = 1;
			break;
		case 2:
			TIE_C2I = 1;
			break;
		case 3:
			TIE_C3I = 1;
			break;
		case 4:
			TIE_C4I = 1;
			break;
		case 5:
			TIE_C5I = 1;
			break;
		case 6:
			TIE_C6I = 1;
			break;
		case 7:
			TIE_C7I = 1;
			break;
	}
			
	return;
}


void tim_disableInterrupts(s8 timId)
{
	if(!IS_VALID_ID(timId)
		return;
	
	switch (timId)
	{
		case 0:
			TIE_C0I = 0;
			break;
		case 1:
			TIE_C1I = 0;
			break;
		case 2:
			TIE_C2I = 0;
			break;
		case 3:
			TIE_C3I = 0;
			break;
		case 4:
			TIE_C4I = 0;
			break;
		case 5:
			TIE_C5I = 0;
			break;
		case 6:
			TIE_C6I = 0;
			break;
		case 7:
			TIE_C7I = 0;
			break;
	}
			
	return;
}


void tim_enableOvfInterrupts(void)
{
	TSCR2 |= 0x80;
	return;
}


void tim_disableOvfInterrupts(void)
{
	TSCR2 &= ~0x80;
	return:
}


void tim_clearFlag(s8 timId)
{
	if(!IS_VALID_ID(timId))
		return;
	
	TFLG1 = 1<<timId;
	
	return;	
}


u16 tim_getvalue(s8 timId)
{
	if(!IS_VALID_ID(timId))
		return 0;
	
	switch (timId)
	{
		case 0:
			return TC0;
		case 1:
			return TC1;
		case 2:
			return TC2;
		case 3:
			return TC3;
		case 4:
			return TC4;
		case 5:
			return TC5;
		case 6:
			return TC6;
		case 7:
			return TC7;
		default:
			return 0; // Nunca pasa
	}
}


void tim_setValue(s8 timId, u16 value)
{
	if(!IS_VALID_ID(timId)
		return;
	
	switch (timId)
	{
		case 0:
			TC0 = value;
			break;
		case 1:
			TC1 = value;
			break;
		case 2:
			TC2 = value;
			break;
		case 3:
			TC3 = value;
			break;
		case 4:
			TC4 = value;
			break;
		case 5:
			TC5 = value;
			break;
		case 6:
			TC6 = value;
			break;
		case 7:
			TC7 = value;
			break;
	}
			
	return;
}


u16 tim_getGlobalValue(void)
{
	return TCNT;
}


void interrupt tim0_srv(void)
{
	if (tim_data.cbArray[0] != NULL)
		(*tim_data.cbArray[0])();
	
	return;
}


void interrupt tim1_srv(void)
{
	if (tim_data.cbArray[1] != NULL)
		(*tim_data.cbArray[1])();
	
	return;
}


void interrupt tim2_srv(void)
{
	if (tim_data.cbArray[2] != NULL)
		(*tim_data.cbArray[2])();
	
	return;
}


void interrupt tim3_srv(void)
{
	if (tim_data.cbArray[3] != NULL)
		(*tim_data.cbArray[3])();
	
	return;
}


void interrupt tim4_srv(void)
{
	if (tim_data.cbArray[4] != NULL)
		(*tim_data.cbArray[4])();
	
	return;
}


void interrupt tim5_srv(void)
{
	if (tim_data.cbArray[5] != NULL)
		(*tim_data.cbArray[5])();
	
	return;
}


void interrupt tim6_srv(void)
{
	if (tim_data.cbArray[6] != NULL)
		(*tim_data.cbArray[6])();
	
	return;
}


void interrupt tim7_srv(void)
{
	if (tim_data.cbArray[7] != NULL)
		(*tim_data.cbArray[7])();
	
	return;
}


void interrupt timOvf_serv(void)
{
	u8 i;
	for (i = 0; i < TIM_AMOUT; i++)
		if (tim_data.ovfArray[i] != NULL)
			(*tim_data.ovfArray[i])();
}


void oc_init(void)
{
	TCTL1 = OC_ACTION1;
	TCTL2 = OC_ACTION2;	// Outputs disconnected
	TIOS |= OC_CHANNEL;	// Reserve OC channel
	OC_INT_ENABLE();	// set desired interrupts
	OC_FLAG_CLR();	// Flag clear
}

void ic_init(void)
{
	TCTL3 = IC_ACTION1;
	TCTL4 = IC_ACTION2;
	TIOS &= ~IC_CHANNEL;	// Reserve IC channel
	IC_INT_ENABLE();
	
	IC_FLAG_CLR();

}