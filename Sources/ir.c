#include "ir.h"

#include "common.h"
#include "timers.h"
#include "cb.h"

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

#define EDGE_TIME_MARGIN 100

#define RC5_TIMEOUT 47500

#define PREVIOUS_BIT ((icData.receivedData & (1<< ( (u8) ( icData.currentBit+1)))) ? 1 : 0) // +1: para volver al previous bit


#define STORE_BIT(a) ((a)? STORE_1() : STORE_0())

static struct {
	u16 lastEdge;
	u16 receivedData;
	s8 currentBit;
	bool running;
	u8 overflowCnt;
	bool icInhibit;
	
}icData = {0, 13, _FALSE, 0};

static u8 irBuffer[BUFF_LENGTH];
static cbuf cBuffer;


void STORE_1(void)
{
	icData.receivedData |= (1 << ((u8) (icData.currentBit)));
	icData.currentBit--;
}

void STORE_0(void)
{
	icData.currentBit--;
}

void ir_init(void)
{
	timer_init();
	ic_init();
	oc_init();
	resetTransmission();	
	
	cBuffer = cb_create(irBuffer, BUFF_LENGTH);
}
/*


void interrupt icIR_srv(void) 	// Elegir channel consistente con IC_CHANNEL ("timers.h")
{	
	icData.icInhibit = _TRUE;
	IC_INT_DISABLE();
	TC0 = TC1 + EDGE_TIME_MARGIN;
	
	if (PORTE_PE6 == 0)
		PORTE_PE6 = 1;
	else
		PORTE_PE6 = 0;
	
	if (icData.running == _FALSE)
	{
		startTransmission();	
	
	}
	else
	{	
		u32 timeElapsed;
	
		timeElapsed = (icData.overflowCnt*CNT_MAX+TC1)-icData.lastEdge;	// nunca tiene que dar <0 la suma parcial.
		
		icData.lastEdge = TC1;
		icData.overflowCnt = 0;
		TC0 = TC1 + RC5_TIMEOUT;
	
		if ((timeElapsed >= HBT2_MIN) && (timeElapsed < HBT2_MAX))
		{
			if (PREVIOUS_BIT == 1)
				STORE_1();
			else
				STORE_0();
		}
		else if ((timeElapsed >= HBT3_MIN) && (timeElapsed < HBT3_MAX))
		{
			if (PREVIOUS_BIT == 0)
				STORE_1();
			else
			{
				STORE_1();
				STORE_0();
			}
		} 
		else if ((timeElapsed >= HBT4_MIN) && (timeElapsed < HBT4_MAX) && (PREVIOUS_BIT == 0))
		{
			STORE_1();
			STORE_0();
		} 
		else 
		{	
			if (PTM_PTM1 == 0)
				PTM_PTM1 = 1;
			else
				PTM_PTM1 = 0;
			
		    resetTransmission(); 
		}
	}
	
	if (icData.currentBit == (-1))
	{	
		endTransmission();
	}
	
}*/
/*
void interrupt ocIR_srv(void) 
{
    OC_FLAG_CLR();
    
    if (icData.icInhibit == _TRUE)
	{
	    icData.icInhibit = _FALSE;
	    IC_FLAG_CLR();
	    IC_INT_ENABLE();
	    TC0 = icData.lastEdge + RC5_TIMEOUT - EDGE_TIME_MARGIN;
    } 
    else
    	resetTransmission();
	
    return;
}*//*

void startTransmission(void)
{
		OVF_FLAG_CLR();
		icData.running = _TRUE;
		IC1_RISING_EDGE;
		icData.currentBit = 13;
		icData.receivedData = 0;
		STORE_1();
		icData.lastEdge = TC1 - HBT_TIME;	// No pasa nada aunque HBT_TIME > TC1
		if (((s32) (TC1 - (s32)(HBT_TIME) )) >= 0)
			icData.overflowCnt = 0;
		else
			icData.overflowCnt = 1;
		
		OC_FLAG_CLR();
		OC_INT_ENABLE(); //Se activa el OC para timeout
		OVF_INT_ENABLE();
}

void resetTransmission(void)
{
		if (PTM_PTM0 == 0)
			PTM_PTM0 = 1;
		else
			PTM_PTM0 = 0;
		
		icData.running = _FALSE;
		IC1_FALLING_EDGE;
		OC_INT_DISABLE();		// Disable output compare int
		OVF_INT_DISABLE();
}

void endTransmission(void){
		u8 data = icData.receivedData & (0x003F);
		data |= (((icData.receivedData & (1<<12)) ? 0 : 1)<<6);
		irPush(data);
		
		disp_ram[0] = (data/1000)%10+'0';
		disp_ram[1] = (data/100)%10+'0';
		disp_ram[2] = (data/10)%10+'0';
		disp_ram[3] = (data)%10+'0';
		

		resetTransmission();
		

		return;
}
*/
/*
void interrupt timOvf_srv(void)
{
	OVF_FLAG_CLR();
	icData.overflowCnt++;

	
	return;
}*/

s16 irPush(u8 data){
	return cb_push(&cBuffer, data);
}

s16 irPop(void){
	return cb_pop(&cBuffer);
}

s16 irFlush(void){
	return cb_flush(&cBuffer);
}

