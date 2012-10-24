#ifndef _IR_H
#define _IR_H

#include "common.h"
#include "cb.h"

void ir_init(void);

s16 ir_push(u8 data);
s16 ir_flush(void);
s16 ir_pop(void);


#endif