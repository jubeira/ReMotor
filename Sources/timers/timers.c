#include "mc9s12xdp512.h"
#include "timers.h"


void timer_init(void) 
{
	TSCR2 = 0x00;
	TSCR2_TOI = TIMER_OVERFLOW_INT;
	TSCR2_TCRE = TIMER_RESET_ENABLE;
	TSCR2 |= TIMER_PRESCALER;	//Prescaler: 4 - de 0000 a FFFF: tarda 10ms en overflow
//	TSCR1_TFFCA = FLAG_AUTOCLR;
	TSCR1_TEN = TIMER_START; // TEN
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
	TIOS &= !IC_CHANNEL;	// Reserve IC channel
	IC_INT_ENABLE();
	
	IC_FLAG_CLR();

}