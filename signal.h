#pragma once

#include "includes.h"

#define SIGSTKSZ 4096

typedef struct stack_t
{
	void* ss_sp;
	size_t ss_size;
	int ss_flags;
} stack_t;


typedef struct siginfo_t
{
	int aaaaaaaaaaa;
} siginfo_t;

//sigset_t stuff
typedef uint32_t sigset_t;

//flags
#define SA_INVALID 0x1 //if sigaction is invalid
#define SA_RESTART 0x2
#define SA_SIGINFO 0x4 

//SIGACTION is very unsafe. Make sure that sa_handler/sa_sigaction finishes before the next timer interrupt =/
struct sigaction 
{
	void    (*sa_handler)(int);
	void     (*sa_sigaction)(int, siginfo_t *, void *);
	sigset_t   sa_mask;
	int        sa_flags;
	void     (*sa_restorer)(void);
};


//signals
enum
{
	SIGALRM = 0x1,
	SIGINT  = 0x2,
	SIGTSTP = 0x4,
	SIGSTOP = 0x8
};

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);

int sigemptyset(sigset_t *set);
int sigfillset(sigset_t *set);
int sigaddset(sigset_t *set, int signo);

#define SIG_BLOCK 1
#define SIG_UNBLOCK 2

int sigprocmask(int how, const sigset_t *set, sigset_t* oldset);


