#ifndef _IR_H
#define _IR_H

void ir_init(void);
//void interrupt icIR_srv(void);
void startTransmission(void);
void resetTransmission(void);
void endTransmission(void);

//void interrupt timOvf_srv(void);
//void interrupt ocIR_srv(void);

s16 irPush(u8 data);
s16 irFlush(void);
s16 irPop(void);


#endif