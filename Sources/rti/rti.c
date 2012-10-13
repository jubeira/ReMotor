/*
 * rti.c
 * 
 * Control de la RTI
 * Lab de micros - 2012 - Grupo X
 *
 */

#include "rti.h"
#include "../derivative.h"
#include "../common.h"

#define NULL ((void*)0)
#define RTI_DIV (0x29)
#define RTI_MAX_FCNS 20


#ifdef DEBUG
#define RTI_DEBUG_DD DDRE_DDRE3
#define RTI_DEBUG_PIN PORTE_PE3

#define RTI_DBG_INIT() (RTI_DEBUG_DD = 1)
#define RTI_DBG_ENTER() (RTI_DEBUG_PIN = 1)
#define RTI_DBG_LEAVE() (RTI_DEBUG_PIN = 0)
#else /* not DEBUG*/
#define RTI_DBG_INIT()
#define RTI_DBG_ENTER()
#define RTI_DBG_LEAVE()
#endif /*DEBUG*/

void rti_reenable(void)
{
	CRGFLG_RTIF = 1;
}

struct rti_cb {
	rti_time period;
	rti_time cnt;
	rti_time (*f)(void *, rti_time);
	int protect;
	void *data;
};

static struct rti_cb rti_tbl[RTI_MAX_FCNS];

void rti_init()
{
	int i;
	
	RTI_DBG_INIT();
	
	for (i = 0; i < RTI_MAX_FCNS; i++)
		rti_tbl[i].f = NULL;
	
	RTICTL = RTI_DIV;
	CRGINT_RTIE = 1;
	rti_reenable();
}

timer_id rti_register2(rti_time (*f)(void *, rti_time), void *data, 
				rti_time period, rti_time delay, int protect)
{
	int i;
	
	for (i = 0; i < RTI_MAX_FCNS; i++) {
		if (rti_tbl[i].f == NULL) {
			rti_tbl[i].f = f;
			rti_tbl[i].data = data;
			rti_tbl[i].period = period;
			rti_tbl[i].protect = protect;
			rti_tbl[i].cnt = delay;
			break;
		}
	}
		
	if (i == RTI_MAX_FCNS)
		i = -E_NOMEM;
	
	return i;
}

timer_id rti_register(rti_time (*f)(void *, rti_time), void *data, 
						rti_time period, rti_time delay)
{
	return rti_register2(f, data, period, delay, RTI_DEFAULT_PROTECT);
}

rti_time rti_set_period(int n, rti_time period)
{
	rti_time old_p = rti_tbl[n].period;
	
	rti_tbl[n].period = period;
	
	return old_p;
}

void rti_cancel(timer_id n)
{
	rti_tbl[n].protect = RTI_NORMAL;
	rti_tbl[n].cnt = RTI_CANCEL;
}

void interrupt rti_srv(void)
{
	int i;

	RTI_DBG_ENTER();

	for (i = 0; i < RTI_MAX_FCNS; i++) {
		if (rti_tbl[i].f != NULL) {
			if (rti_tbl[i].cnt == RTI_AUTOCANCEL) {
				if (!rti_tbl[i].protect == RTI_PROTECT)
					rti_tbl[i].f = NULL;
			} else {
				if(!(--rti_tbl[i].cnt)) {
					rti_tbl[i].period = rti_tbl[i].f(rti_tbl[i].data, rti_tbl[i].period);
					rti_tbl[i].cnt = rti_tbl[i].period;
				}
			}
		}
	}
	
	RTI_DBG_LEAVE();
	rti_reenable();
}

/* funciones convenientes */

rti_time flagger(void *flag, rti_time pw)
{
	*((u8*)flag) = 1;
	
	return pw;
}
