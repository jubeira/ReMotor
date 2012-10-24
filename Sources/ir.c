#include "ir.h"
#include "timers.h"

#define BUFF_LENGTH 50
#define CNT_MAX ((u32)65536)
#define HBT_TIME ((u32)8890)

#define HBT1_MAX ((u32)9336)
#define HBT1_MIN ((u32)8446)
#define HBT2_MAX ((u32)18668)
#define HBT2_MIN ((u32)16893)
#define HBT3_MAX ((u32)28001)
#define HBT3_MIN ((u32)25339)
#define HBT4_MAX ((u32)37335)
#define HBT4_MIN ((u32)33785)

#define RC5_TIMEOUT 47500
#define EDGE_TIME_MARGIN 100

#define PREVIOUS_BIT() ((icData.receivedData & (1<< ( (u8) ( icData.currentBit+1)))) ? 1 : 0) // +1: para volver al previous bit

static struct 
{
	u16 lastEdge;
	u16 receivedData;
	s8 currentBit;
	bool transmitting;
	u8 overflowCnt;
	bool icInhibit;
	bool init;
} icData = {0,0, 13, _FALSE, 0,_FALSE, _FALSE};

static struct 
{
	s8 icTimerId;
	s8 ocTimerId;
} irTimers;


static u8 irBuffer[BUFF_LENGTH];
static cbuf cBuffer;


void startTransmission(void);
void resetTransmission(void);
void endTransmission(void);

void store_0(void);
void store_1(void);

void ir_icSrv(void);
void ir_ocSrv(void);
void ir_ovfSrv(void);

void store_1(void)
{
	icData.receivedData |= (1 << ((u8) (icData.currentBit)));
	icData.currentBit--;
}


void store_0(void)
{
	icData.currentBit--;
}


void ir_init(void)
{
	if (icData.init == _TRUE) 
		return;
	
	icData.init = _TRUE;
	
	timer_init();
	
	irTimers.icTimerId = tim_getTimer(TIM_IC, ir_icSrv, ir_ovfSrv);
	irTimers.ocTimerId = tim_getTimer(TIM_OC, ir_ocSrv, NULL);
	
	tim_enableOvfInterrupts(irTimers.icTimerId);
	
	cBuffer = cb_create(irBuffer, BUFF_LENGTH);	
	
	resetTransmission();	
	
	return;
}


void resetTransmission(void)
{
	icData.transmitting = _FALSE;
	
	//Las interrupciones por OC s�lo est�n habilitadas durante una transmisi�n
	icData.icInhibit = _FALSE;
	tim_enableInterrupts(irTimers.icTimerId); 
	tim_setFallingEdge(irTimers.icTimerId);
	tim_disableInterrupts(irTimers.ocTimerId);
}


void startTransmission(void)
{
	icData.transmitting = _TRUE;
	
	tim_setRisingEdge(irTimers.icTimerId);
	
	icData.currentBit = 14;
	icData.receivedData = 0;
	store_1();
	
	icData.lastEdge = tim_getValue(irTimers.icTimerId) - HBT_TIME;	// No pasa nada aunque HBT_TIME > TC1
	
	if (((s32) (tim_getValue(irTimers.icTimerId) - (s32)(HBT_TIME) )) >= 0)
		icData.overflowCnt = 0;
	else
		icData.overflowCnt = 1;
	
	tim_enableInterrupts(irTimers.ocTimerId);
}


void endTransmission(void)
{
	u8 data = icData.receivedData & (0x003F);
	data |= (((icData.receivedData & (1<<12)) ? 0 : 1)<<6);	// ASK HUGO BOSS
	ir_push(data);

	resetTransmission();	

	return;
}


void ir_icSrv(void) 
{		
	icData.icInhibit = _TRUE;
	tim_disableInterrupts(irTimers.icTimerId);
	
	tim_clearFlag(irTimers.ocTimerId);
	tim_setValue(irTimers.ocTimerId, tim_getValue(irTimers.icTimerId) + EDGE_TIME_MARGIN); //Margen por rise time lento

	if (icData.transmitting == _FALSE)
		startTransmission();	
	else
	{	
		u32 timeElapsed = (icData.overflowCnt * CNT_MAX + tim_getValue(irTimers.icTimerId)) - icData.lastEdge;
		
		icData.lastEdge = tim_getValue(irTimers.icTimerId);
		icData.overflowCnt = 0;
	
		if ((timeElapsed >= HBT2_MIN) && (timeElapsed < HBT2_MAX))
		{
			if (PREVIOUS_BIT() == 1)
				store_1();
			else
				store_0();
		}
		else if ((timeElapsed >= HBT3_MIN) && (timeElapsed < HBT3_MAX))
		{
			if (PREVIOUS_BIT() == 0)
				store_1();
			else
			{
				store_1();
				store_0();
			}
		} 
		else if ((timeElapsed >= HBT4_MIN) && (timeElapsed < HBT4_MAX) && (PREVIOUS_BIT() == 0))
		{
			store_1();
			store_0();
		} 
		else 
		    resetTransmission(); 
	}
	
	if (icData.currentBit == (-1))
		endTransmission();
	
	return;
}


void ir_ocSrv(void) 
{
    if (icData.icInhibit == _TRUE)
	{
	    icData.icInhibit = _FALSE;
	    tim_clearFlag(irTimers.icTimerId);
	    tim_enableInterrupts(irTimers.icTimerId);
	    tim_setValue(irTimers.ocTimerId, (icData.lastEdge + RC5_TIMEOUT) - EDGE_TIME_MARGIN);
    } 
    else
    	resetTransmission();
	
    return;
}


void ir_ovfSrv(void)
{
	icData.overflowCnt++;
	
	return;
}


s16 ir_push(u8 data)
{
	return cb_push(&cBuffer, data);
}


s16 ir_pop(void)
{
	return cb_pop(&cBuffer);
}


s16 ir_flush(void)
{
	return cb_flush(&cBuffer);
}

