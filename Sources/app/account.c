/*
* account.c
* 
* Control de LED
* Lab de micros - 2012 - Grupo X
*
*/

#include "account.h"

#define USERS 3

struct account acc_db[USERS] = 
{
{ "1234","5678"},
{ "4455","10"},
{ "0",	""}
};

s16 is_valid_account(s8 *id, s8 *pw)
{
	s16 i;
	for (i = 0; i < USERS; i++)
		if ((!strcmp(acc_db[i].id,id)) && (!strcmp(acc_db[i].pw,pw)))
			return 1;
		
	return 0;
}