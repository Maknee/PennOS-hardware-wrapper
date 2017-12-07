#pragma once

#include "includes.h"
#include "isr.h"
#include "signal.h"

typedef struct cpu_registers
{
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;

	uint32_t esp;
	uint32_t ss;
	uint32_t ds;
	
	uint32_t eip;
	uint32_t eflags;
	uint32_t cs;
} cpu_registers;

typedef struct ucontext
{
	struct ucontext *uc_link;
	stack_t uc_stack;
	cpu_registers registers;
	sigset_t uc_sigmask;
} ucontext_t;

int getcontext(ucontext_t *ucp);
int setcontext(const ucontext_t *ucp);
void makecontext(ucontext_t *ucp, void (*func)(), int argc, ...);
int swapcontext(ucontext_t *oucp, const ucontext_t *ucp);


