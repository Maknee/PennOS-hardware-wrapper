#include "signal.h"

extern struct sigaction pit_sigaction;
extern struct sigaction sigint_sigaction;
extern struct sigaction sigtstp_sigaction;

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact)
{
	if(signum == SIGALRM)
	{
		pit_sigaction = (struct sigaction)
		{
		.sa_handler = act->sa_handler,
		.sa_sigaction = act->sa_sigaction,
		.sa_mask = act->sa_mask,
		.sa_flags = act->sa_flags,
		.sa_restorer = act->sa_restorer
		};
	}
	else if(signum == SIGINT)
	{
		sigint_sigaction = (struct sigaction)
		{
		.sa_handler = act->sa_handler,
		.sa_sigaction = act->sa_sigaction,
		.sa_mask = act->sa_mask,
		.sa_flags = act->sa_flags,
		.sa_restorer = act->sa_restorer
		};
	}
	else if(signum == SIGTSTP)
	{
		sigtstp_sigaction = (struct sigaction)
		{
		.sa_handler = act->sa_handler,
		.sa_sigaction = act->sa_sigaction,
		.sa_mask = act->sa_mask,
		.sa_flags = act->sa_flags,
		.sa_restorer = act->sa_restorer
		};
	}

	//return sucessful
	return 0;
}

int sigemptyset(sigset_t *set)
{
	//null all bytes
	memset(set, 0, sizeof(sigset_t));
	return 0;
}

int sigfillset(sigset_t *set)
{
	//fill all bytes
	memset(set, 0xff, sizeof(sigset_t));
	return 0;
}

int sigaddset(sigset_t *set, int signo)
{
	//set the bit
	*set |= signo;
	return 0;
}

//pit.c
extern bool pit_sigaction_blocked;

//keyboard.c
extern bool sigint_sigaction_blocked;
extern bool sigtstp_sigaction_blocked;

int sigprocmask(int how, const sigset_t *set, sigset_t* oldset)
{
	//makes no difference
	/*
	switch(how)
	{
		case SIG_BLOCK:
		{
			if(*set & SIGALRM)
			{
				pit_sigaction_blocked = true;
			}
			if(*set & SIGINT)
			{
				sigint_sigaction_blocked = true;
			}
			if(*set & SIGTSTP || *set & SIGSTOP)
			{
				sigtstp_sigaction_blocked = true;
			}
			break;
		}
		case SIG_UNBLOCK:
		{
			if(*set & SIGALRM)
			{
				pit_sigaction_blocked = false;
			}
			if(*set & SIGINT)
			{
				sigint_sigaction_blocked = false;
			}
			if(*set & SIGTSTP || *set & SIGSTOP)
			{
				sigtstp_sigaction_blocked = false;
			}
			break;
		}
		default:
			break;
	}
	*/
	return 0;
}



