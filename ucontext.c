#include "ucontext.h"

extern int getcontext(ucontext_t *ucp);

extern int setcontext(const ucontext_t *ucp);

extern void makecontext(ucontext_t *ucp, void (*func)(), int argc, ...);

//https://stackoverflow.com/questions/19469000/implementing-swapcontext-in-terms-of-getcontext-and-setcontext

int swapcontext(ucontext_t *oucp, const ucontext_t *ucp)
{
	volatile bool swapped = false;
	int result = getcontext(oucp);
	if (result == 0 && !swapped) 
	{
		swapped = true;
		result = setcontext(ucp);
	}

	return result;
}
