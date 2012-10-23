#ifndef __KB_H__
#define __KB_H__

#define KB_SPECIAL_L 'A'
#define KB_SPECIAL_R 'B'

#define KB_ACTIVE	1

extern s16 kb_active;

void kb_init(void);

void kb_activate(s16 n);

s16 kb_status();

s16 kb_read();

void kb_flush();

#endif