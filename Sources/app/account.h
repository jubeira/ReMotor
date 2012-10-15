/*
* account.h
* 
* Control de LED
* Lab de micros - 2012 - Grupo X
*
*/

#define ID_LEN 5
#define PW_LEN 5

struct account 
{
	s8 id[ID_LEN];
	s8 pw[PW_LEN];
};

s16 is_valid_account(s8 *id, s8 *pw);
