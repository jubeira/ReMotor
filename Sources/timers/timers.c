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

#define OC_INT_DISABLE() (TIE_C0I = 0)		// Disable output compare int
#define OVF_INT_DISABLE() (TSCR2 &= ~0x80)
#define OC_INT_ENABLE()	(TIE_C0I = 1)
#define OVF_INT_ENABLE() (TSCR2 |= 0x80)
#define IC_INT_ENABLE() (TIE_C1I = 1)
#define IC_INT_DISABLE() (TIE_C1I = 0)

#define IC_FLAG_CLR() (TFLG1=0x02)

#define IC1_FALLING_EDGE {TCTL4_EDG1A = 0;TCTL4_EDG1B = 1;}
#define IC1_RISING_EDGE {TCTL4_EDG1A = 1;TCTL4_EDG1B = 0;}

void oc_init(void);
void ic_init(void);

struct {
 bool init;
 bool t0Used;
 bool t1Used;
 bool t2Used;
 bool t3Used;
 bool t4Used;
 bool t5Used;
 bool t6Used;
 bool t7Used;
} tim_data;

void timer_init(void) 
{
	if (tim_data.init == _FALSE)
	{
		tim_data.init = true;
		
		TSCR2 = 0x00;
		TSCR2_TOI = TIMER_OVERFLOW_INT;
		TSCR2_TCRE = TIMER_RESET_ENABLE;
		TSCR2 |= TIMER_PRESCALER;	//Prescaler: 4 - de 0000 a FFFF: tarda 10ms en overflow
		TSCR1_TEN = TIMER_START; // TEN
	}
	
	return;
}

s8 timId tim_getTimer(tim_type reqType, tim_ptr intFunct);
void tim_freeTimer(s8 timId);

void tim_setFallingEdge(s8 timId);
void tim_setRisingEdge(s8 timId);
void tim_setBothEdge(s8 timId);

void tim_enableInterrupts(s8 timId);
void tim_disableInterrupts(s8 timId);

void tim_clearFlag(s8 timId);

void tim_getvalue(s8 timId);
void tim_setValue(s8 timId);

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