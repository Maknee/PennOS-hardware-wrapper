#include "time.h"

//pit.c
extern struct itimerval pit_itimerval;

int setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value)
{
	if(which == ITIMER_REAL)
	{
		memcpy(&pit_itimerval, new_value, sizeof(struct itimerval));
		return 0;
	}
	return -1;
}


