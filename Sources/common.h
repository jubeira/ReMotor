#ifndef _LIBCOMMON_H_
#define _LIBCOMMON_H_

#ifndef NULL
#define NULL ((void*)0)
#endif

#define DIVUP(a,b) (((a)/(b))+1)
#define MAKE_EVEN(a) (((x)/2)*2)

#define ARSIZE(a) ((sizeof(a))/(sizeof((a)[0])))

#define BOOL(a) ((a)?1:0)

enum {E_OK, E_NOMEM, E_BADCFG, E_GRAPHIC, E_DONEHELP, E_BADARGS, E_NOTREADY,
					E_TIMEOUT, E_OTHER, N_ECODES}; 

#define SUCCESS (-E_OK)

#define BIT(v, i) ((v) & (1<<i))

typedef unsigned char u8;
typedef char s8;
typedef unsigned int u16;
typedef int s16;
typedef unsigned long int u32;
typedef long int s32;

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

 typedef enum{
	_FALSE = 0,
	_TRUE
}bool;


#endif
