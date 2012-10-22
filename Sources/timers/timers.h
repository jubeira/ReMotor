#ifndef _TIMERS_H
#define _TIMERS_H

#include "mc9s12xdp512.h"

#define INVALID_TIMER (-1)

typedef enum
{
	TIM_IC,
	TIM_OC
} tim_type;

typedef void (*tim_ptr) (void)

void timer_init (void);

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





#endif // "_TIMERS_H"