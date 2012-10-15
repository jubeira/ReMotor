#ifndef _TIMERS_H
#define _TIMERS_H

#include "mc9s12xdp512.h"

#define TIMER_PRESCALER 2			// [0,7]
#define TIMER_START 1					// {0,1}
#define TIMER_OVERFLOW_INT 1	// {0,1}
#define TIMER_RESET_ENABLE 0	// {0,1}
#define FLAG_AUTOCLR 1				// {0,1}
#define OVF_FLAG_CLR (TFLG2=0x80)


#define OC_FLAG_CLR (TFLG1=0x01)
#define OC_CHANNEL (1<<0)				// Mask - must not use IC_CHANNEL bits
#define OC_INT_ENABLE (1<<0)		    // Mask - must not use IC_INT_ENABLE bits
#define OC_ACTION1 0x00				// Mask
#define OC_ACTION2 0x00				// Mask

#define OC0_DISCONNECTED {TCTL2_OL0=0;TCTL2_OM0=0;}


#define IC_ACTION1 0x00
#define IC_ACTION2 0x04
#define IC_CHANNEL 1<<1
#define IC_INT_ENABLE 1<<1
#define IC_FLAG_CLR {TFLG1=0x01;}

#define IC1_FALLING_EDGE {TCTL4_EDG1A = 0;TCTL4_EDG1B = 1;}
#define IC1_RISING_EDGE {TCTL4_EDG1A = 1;TCTL4_EDG1B = 0;}

void timer_init(void);
void oc_init(void);
void ic_init(void);


#endif // "_TIMERS_H"