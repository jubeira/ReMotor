/*
 *      common.h
 *      
 *	Cosas que vas a necesitar
 *  Grupo 2 - 2012
 */
#ifndef _LIBCOMMON_H_
#define _LIBCOMMON_H_

#define DEBUG
#define test 1
#ifndef NULL
#define NULL ((void*)0)
#endif

/* cosas */

#define DIVUP(a,b) (((a)/(b))+1)
#define MAKE_EVEN(a) (((x)/2)*2)

/* #define PERROR(msg...) fprintf(stderr, msg) */
#define ARSIZE(a) ((sizeof(a))/(sizeof((a)[0])))
#define __MALLOC(S) (S = malloc(sizeof(*(S))))
#define NMALLOC(S, n) (S = malloc((n)*sizeof(*(S))))
#define BOOL(a) ((a)?1:0)

enum {E_OK, E_NOMEM, E_BADCFG, E_GRAPHIC, E_DONEHELP, E_BADARGS, E_NOTREADY,
					E_TIMEOUT, E_OTHER, N_ECODES}; 

#define SUCCESS (-E_OK)

/* BITWISE */

#define BIT(v, i) ((v) & (1<<i))

/*TYPEDEFS */

typedef unsigned char u8;
typedef unsigned int uint;
typedef union {
	u8 v;
	struct {
		int b0:1;
		int b1:1;
		int b2:1;
		int b3:1;
		int b4:1;
		int b5:1;
		int b6:1;
		int b7:1;
	};
} bit;

/* puertos */

#define PORTDD_OUT 1

/* Warnings */

#ifdef DEBUG
#warning Building with DEBUG options
#endif

#endif /* _LIBCOMMON_H_ */
