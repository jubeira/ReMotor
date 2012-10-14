/*
 * console.c
 * 
 * Grupo 2 - 2012
 * 
 * Intento de consola
 */

#include <string.h>
#include "util/cb.h"
#include "placa/kbd.h"
#include "placa/display.h"
#include "console.h"

/* disp control */

#define PW_CHAR ('-')
#define CURSOR_CHAR ('_')
#define TERM ('\0')
#define CHAR_NOTHING (' ')
#define ATT_CURSOR ATT_BLINK

/* input control*/

#define BKSPACE	KB_SPECIAL_L
#define ENTER	KB_SPECIAL_R

/* */

s8 con_inp_buf[DISP_SIZE];
static s16 print_mode;
static s16 cursor;
s16 input_ready;
static s16 readfrom;
static s16 cursor_enabled;
static s16 Gtimeout;
static timer_id timeout_timer;
static rti_time t_out;

rti_time con_service(void *dummy, rti_time pw);

void con_init(void)
{
	display_init();
	kb_init();
	con_input_enable(BLOCK_INPUT);
	
	Gtimeout = 1;
	t_out = 0;
	timeout_timer = RTI_INVALID_ID;
	
	con_clear();
	con_output_mode(CON_NORMAL);
	rti_register(con_service, NULL, RTI_MS2PERIOD(100), RTI_NOW);
}

void con_clear()
{
/*	memset(con_inp_buf, SPACE, ARSIZE(con_inp_buf)); */
	memset(disp_ram, CHAR_NOTHING, DISP_SIZE);
	
	cursor = 0;
	readfrom = 0;
	input_ready = 0;
}

static void low_setattr(u8 attrs, s16 mode, s16 n)
{
	switch (mode) {
	case ATT_SET:
		disp_att_ram[n].dim = BOOL(attrs & ATT_DIM);
		disp_att_ram[n].use_dot = BOOL(attrs & ATT_DOT);
		disp_att_ram[n].blink = BOOL(attrs & ATT_BLINK);
		break;
	case ATT_ADD:
		disp_att_ram[n].dim |= BOOL(attrs & ATT_DIM);
		disp_att_ram[n].use_dot |= BOOL(attrs & ATT_DOT);
		disp_att_ram[n].blink |= BOOL(attrs & ATT_BLINK);
		break;
	case ATT_CLEAR:
		disp_att_ram[n].dim &= !BOOL(attrs & ATT_DIM);
		disp_att_ram[n].use_dot &= !BOOL(attrs & ATT_DOT);
		disp_att_ram[n].blink &= !BOOL(attrs & ATT_BLINK);
		break;
	default : break;
	}
}

static s16 low_getchar(void)
{
	s16 c;
	
	if (input_ready) {
		if (readfrom == cursor) {
			c = TERM;
			input_ready = 0;
		} else {
			c = con_inp_buf[readfrom++];
		}
	} else {
		c = -E_NOTREADY;
	}
	
	return c;
}

static s16 low_putcursor(void)
{
	s16 r = 0;
	
	if (cursor < DISP_SIZE && cursor_enabled) {
		disp_ram[cursor] = CURSOR_CHAR;
		low_setattr(ATT_BLINK, ATT_SET, cursor);
		r = 1;
	}
	
	return r;
}

static s16 low_rmcursor(void)
{
	s16 r = 0;
	
	if (cursor < DISP_SIZE) {
		disp_ram[cursor] = CHAR_NOTHING;
		low_setattr(ATT_CURSOR, ATT_CLEAR, cursor);
		r = 1;
	}
	
	return r;
}

static s16 low_putchar(s8 c)
{
	s16 r = 0;
	
	if (cursor < DISP_SIZE) {
		con_inp_buf[cursor] = c;
		low_setattr(ATT_CURSOR, ATT_CLEAR, cursor);
		disp_ram[cursor] = (print_mode == CON_PASSWD)? PW_CHAR : c;
		cursor ++;
		low_putcursor();
		r = 1;
	}
	
	return r;
}

static s16 low_backspace(void)
{
	s16 r = 0;
	
	if (cursor > 0) {
		if (cursor < DISP_SIZE)
			disp_ram[cursor] = CHAR_NOTHING;
		//disp_ram[--cursor] = CHAR_NOTHING;
		cursor--;
		low_putcursor();
		r = 1;
	}
	
	return r;
}

static void timeout_restart()
{
	if (timeout_timer >= 0) {
			rti_cancel(timeout_timer);
			timeout_timer = RTI_INVALID_ID;
	}
	if (!Gtimeout) {
			timeout_timer = rti_register2(flagger, &Gtimeout, RTI_ONCE, t_out, RTI_PROTECT);
	}
}

void con_setattr(u8 attrs, s16 mode)
{
	if (cursor > 0)
		low_setattr(attrs, mode, cursor-1);
}

s16 con_putchar(s8 c) { return low_putchar(c); }

s16 con_puts(s8 *s)
{
	s8 *s2 = s;
	
	for (/* */ ; (*s2 != TERM) && ((s2-s) < DISP_SIZE); s2++)
		con_putchar(*s2);
	
	return s2-s;
}

s16 con_getchar()
{
	return con_getchar_timed(0);
}

s8 *con_gets_timed(s8 *s, rti_time timeout)
{
	s8 *r = NULL;
	
	if (timeout != 0) {
		t_out = timeout;
		Gtimeout = 0;
		timeout_restart();
	}
	
	while (timeout ==0 || !Gtimeout) {
		if (input_ready) {
			s16 n;
			
			n = cursor - readfrom;
			memcpy(s, con_inp_buf + readfrom , n);
			s[n] = '\0';
			r = s;
			
			break;
		}
	}
	
	return r;	
}

s8 *con_gets(s8 *s)
{
	return con_gets_timed(s, 0);
}

void con_cursor_enable(s16 v)
{
	cursor_enabled = v;
	if (cursor_enabled)
		low_putcursor();
	else
		low_rmcursor();
}
u8 b = 50;
rti_time con_service(void *dummy, rti_time pw)
{
	s16 c;
		
	if (!input_ready && (c = kb_read()) >= 0) {
		timeout_restart();
		
		switch (c) {
		case BKSPACE:
			low_backspace();
			break;
		case ENTER:
			con_input_enable(BLOCK_INPUT);
			kb_flush();
			input_ready = 1;
			break;
		default:
			low_putchar(c);
			break;
		}
	}
	
	return pw;
}

s16 con_nchars(void)
{	/* cantidad de caracteres en el buffer */
	return cursor - readfrom;
}

void con_output_mode(s16 m)
{
	print_mode = m;
}

void con_input_enable(s16 v)
{
	if (v == BLOCK_INPUT) {
		con_cursor_enable(0);
		kb_activate(0);
	} else {
		con_cursor_enable(1);
		kb_flush();
		kb_activate(1);
	}
}
