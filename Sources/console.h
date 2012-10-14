/*
 * console.h
 * 
 * Grupo 2 - 2012
 */

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "common.h"
#include "rti/rti.h"

/* display control */

enum {CON_NORMAL, CON_PASSWD};
enum {ATT_DIM_BIT, ATT_DOT_BIT, ATT_BLINK_BIT};
enum {ATT_SET, ATT_ADD, ATT_CLEAR};

#define ATT_DIM (1<<ATT_DIM_BIT)
#define ATT_DOT (1<<ATT_DOT_BIT)
#define ATT_BLINK (1<<ATT_BLINK_BIT)
#define CON_TIMEOUT	NULL

/* input control*/

enum {BLOCK_INPUT, ALLOW_INPUT};

/* funciones */

void con_init(void);
void con_clear();
void con_input_enable(s16 v);
	/* BLOCK_INPUT o ALLOW_INPUT */
void con_output_mode(s16 m);
	/* CON_NORMAL o CON_PASSWD */
void con_setattr(u8 attrs, s16 mode);

s16 con_putchar(s8 c);

s16 con_puts(s8 *s);

s16 con_getchar_timed(rti_time timeout);

s16 con_getchar();

s8 *con_gets_timed(s8 *s, rti_time timeout);

s8 *con_gets(s8 *s);

void con_cursor_enable(s16 v);

s16 con_nchars(void);

#endif /* __CONSOLE_H__ */
