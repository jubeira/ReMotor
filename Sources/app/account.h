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
	char id[ID_LEN];
	char pw[PW_LEN];
};

int is_valid_account(char *id, char *pw);
