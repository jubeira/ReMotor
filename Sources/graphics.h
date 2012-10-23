#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include "rti.h"
#include "display.h"

#define fancy_msg(err_msg) (disp_blinking_message(err_msg,RTI_MS2PERIOD(3000), RTI_MS2PERIOD(3000),RTI_MS2PERIOD(250), RTI_MS2PERIOD(300))) 

void disp_blinking_message (s8* msg, rti_time dim_dur, rti_time blink_dur, rti_time dim_rate, rti_time blink_rate);

#endif