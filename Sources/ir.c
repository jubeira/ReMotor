#include "common.h"
#include "timers.h"
#include "cb.h"
#include "ir.h"
#include "display.h"
#include "timers.h"

#define BUF_LENGTH 50
#define CNT_MAX ((u16)65536)
#define HBT_TIME ((u16)8890)

#define HBT1_MAX ((u16)9336)
#define HBT1_MIN ((u16)8446)
#define HBT2_MAX ((u16)18668)
#define HBT2_MIN ((u16)16893)
#define HBT3_MAX ((u16)28001)
#define HBT3_MIN ((u16)25339)
#define HBT4_MAX ((u16)37335)
#define HBT4_MIN ((u16)33785)

#define CLEAR_IC_FLAG() (TFLG1 = TFLG1_C1F_MASK)
#define CLEAR_OC_FLAG() (TFLG1 = TFLG1_C0F_MASK)


#define RC5_TIMEOUT 37500

#define PREVIOUS_BIT ((icData.receivedData & (1<< ( (u8) ( icData.currentBit+1)))) ? 1 : 0)
	// +1: para volver al previous bit
#define STORE_1() (icData.receivedData |= (1 << ((u8) (icData.currentBit--))))
#define STORE_0() (icData.currentBit--)

#define STORE_BIT(a) ((a)? STORE_1() : STORE_0())

static struct {
	u16 lastEdge;
	u16 receivedData;
	s8 currentBit;
	bool running;
	u8 overflowCnt;
	
}icData = {0, 13, _FALSE, 0};

static u8 irBuffer[BUF_LENGTH];
static cbuf cBuffer;


void ir_init(void){
	ic_init();
	oc_init();
	resetTransmission();
	timer_init();	
	DDRE_DDRE6 = 1;
	PORTE_PE6 = 0;
	
	cBuffer = cb_create(irBuffer, BUF_LENGTH);
}



void interrupt icIR_srv(void){		// Elegir channel consistente con IC_CHANNEL ("timers.h")
	u32 timeElapsed;
	CLEAR_IC_FLAG();
	
	if (icData.running == _FALSE)
	{
		startTransmission();
	}	
	else
	{
		timeElapsed = (icData.overflowCnt*CNT_MAX+TC1)-icData.lastEdge;	// nunca tiene que dar <0 la suma parcial.

		icData.overflowCnt = 0;
		TC0 = TC1 + RC5_TIMEOUT;
	
		if (timeElapsed >= HBT2_MIN && timeElapsed < HBT2_MAX)
		{
			if (PREVIOUS_BIT == 1)
				STORE_1();
			else
				STORE_0();
		}
		else if (timeElapsed >= HBT3_MIN && timeElapsed < HBT3_MAX)
		{
			if (PREVIOUS_BIT == 0)
				STORE_1();
			else
			{
				STORE_1();
				STORE_0();
			}
		} 
		else if (timeElapsed >= HBT4_MIN && timeElapsed < HBT4_MAX && PREVIOUS_BIT == 0)
		{
			STORE_1();
			STORE_0();
		} 
		else 
		{
		    resetTransmission(); 
		}
	}
		
	if (icData.currentBit == -1)
		endTransmission();
	
}

void interrupt ocIR_srv(void) {
    OC_FLAG_CLR();
    resetTransmission();
    return;
}

void startTransmission(void){
		
		OVF_FLAG_CLR();
		icData.running = _TRUE;
		IC1_RISING_EDGE;
		icData.currentBit = 13;
		icData.receivedData = 0;
		STORE_1();
		icData.lastEdge = TC1 - HBT_TIME;	// No pasa nada aunque HBT_TIME > TC1
		if (((s16) (TC1 - HBT_TIME )) >= 0)
			icData.overflowCnt = 0;
		else
			icData.overflowCnt = 1;
		
		TC0 = TC1 + RC5_TIMEOUT;
		OC_FLAG_CLR();
		OC_INT_ENABLE(); //Se activa el OC para timeout
		OVF_INT_ENABLE();
}

void resetTransmission(void){
		icData.running = _FALSE;
		IC1_FALLING_EDGE;
		OC_INT_DISABLE();		// Disable output compare int
		OVF_INT_DISABLE();
}

void endTransmission(void){
		u8 data = icData.receivedData & (0x003F);
		data |= (((icData.receivedData & (1<<12)) ? 0 : 1)<<6);
		irPush(data);
		resetTransmission();
		return;
}


void interrupt timOvf_srv(void)
{
	icData.overflowCnt++;
	if (PORTE_PE6 == 0)
		PORTE_PE6 = 1;
	else
		PORTE_PE6 = 0;
	OVF_FLAG_CLR();
	
	return;
}

s16 irPush(u8 data){
	return cb_push(&cBuffer, data);
}

s16 irPop(void){
	return cb_pop(&cBuffer);
}

s16 irFlush(void){
	return cb_flush(&cBuffer);
}

