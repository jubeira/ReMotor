#include "common.h"
#include "timers.h"
#include "cb.h"
#include "ir.h"
#include "display.h"
#include "timers.h"

#define BUF_LENGTH 50
#define CNT_MAX 65536
#define HBT_TIME 8890

#define HBT1_MAX 9336
#define HBT1_MIN 8446
#define HBT2_MAX 18668
#define HBT2_MIN 16893
#define HBT3_MAX 28001
#define HBT3_MIN 25339
#define HBT4_MAX 37335
#define HBT4_MIN 33785

#define CLEAR_IC_FLAG() (TFLG1 = TFLG1_C1F_MASK)
#define CLEAR_OC_FLAG() (TFLG1 = TFLG1_C0F_MASK)


#define RC5_TIMEOUT 37500

#define PREVIOUS_BIT (icData.receivedData & (1<< ( (u8) ( icData.currentBit+1))))
	// +1: para volver al previous bit
#define STORE_BIT (icData.receivedData |= (1 << ((u8) icData.currentBit)))
	// Reminder: los ceros no se guardan; receivedData se inicializa en 0
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
	
	if (icData.running == _FALSE){
		startTransmission();
	}	
	else{
		timeElapsed = (icData.overflowCnt*CNT_MAX+TC2)-icData.lastEdge;	// nunca tiene que dar <0 la suma parcial.

		icData.overflowCnt = 0;
		
		if (timeElapsed >= HBT2_MIN && timeElapsed < HBT2_MAX){
			if (PREVIOUS_BIT == 1)
				STORE_BIT;
			// else: se guarda un 0 (correr el cursor)
			icData.currentBit--;
		}
		else if (timeElapsed >= HBT3_MIN && timeElapsed < HBT3_MAX){
			if (PREVIOUS_BIT == 0)
				STORE_BIT;
			else{
				STORE_BIT;
				icData.currentBit--;
			}
			icData.currentBit--;
		} 
		else if (timeElapsed >= HBT4_MIN && timeElapsed < HBT4_MAX 
				&& PREVIOUS_BIT == 0){
			STORE_BIT;
			icData.currentBit--;
			icData.currentBit--;																				
		} 
		else {
		    resetTransmission(); 
		    }
	}
		
	TC0 = TCNT + RC5_TIMEOUT;
	
	if (icData.currentBit == -1){
		endTransmission();
	}
	
}

void interrupt ocIR_srv(void) {
    CLEAR_OC_FLAG();
    resetTransmission();
    return;
}

void startTransmission(void){
		icData.running = _TRUE;
		IC1_RISING_EDGE;
		icData.currentBit = 13;
		icData.receivedData = 0;
		icData.receivedData |= (1 << icData.currentBit);
		icData.lastEdge = TC1 - HBT_TIME;
		if (((s16) (TC1 - HBT_TIME )) >= 0)
			icData.overflowCnt = 0;
		else
			icData.overflowCnt = 1;
		icData.currentBit--;					// Se deja para el próximo el current
		
		TC0 = TCNT + RC5_TIMEOUT;
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
		cb_push(&cBuffer, data);
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

