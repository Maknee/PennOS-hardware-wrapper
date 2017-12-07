#pragma once

#include "includes.h"
#include "string.h"

//https://linux.die.net/man/2/setitimer

#define ITIMER_REAL 0

typedef uint32_t time_t;
typedef uint32_t suseconds_t;

struct timeval {
	time_t      tv_sec;         /* seconds */
	suseconds_t tv_usec;        /* microseconds THIS IS THE ONLY IMPORTANT ONE*/ 
};

struct itimerval {
	struct timeval it_interval; /* next value */
	struct timeval it_value;    /* current value */
};

int setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value);


