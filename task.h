#pragma once

#include "includes.h"
#include "isr.h"
#include "signal.h"
#include "gdt.h"

#define TOTAL_TASKS 256

/* Struct which aggregates many registers */
typedef struct task_t
{
	uint8_t stack[SIGSTKSZ];
	struct interrupt_registers* registers;
} task_t;

bool install_task(task_t* task, void* function);

void init_task();

struct interrupt_registers* schedule_next_task(struct interrupt_registers* registers);


