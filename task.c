#include "task.h"

static task_t* tasks[TOTAL_TASKS];

//number of tasks
static int32_t num_tasks = 0;

static int32_t current_task_index = -1;

bool install_task(task_t* task, void* function)
{
	if(num_tasks >= TOTAL_TASKS)
	{
		return false;
	}
	else
	{
		//install the default register stack on task stack
		task->registers = (interrupt_registers*)(task->stack + SIGSTKSZ - sizeof(*task->registers));

		//initialize cpu state
		task->registers->ds = GDT_DATA_SEGMENT;

		task->registers->edi = 0;
		task->registers->esi = 0;
		task->registers->ebp = 0;
		task->registers->ebx = 0;
		task->registers->edx = 0;
		task->registers->ecx = 0;
		task->registers->eax = 0;

		task->registers->eip = (uint32_t)function;
		task->registers->cs = GDT_CODE_SEGMENT;
		task->registers->eflags = 0x202;

		//install the current task
		tasks[num_tasks++] = task;
		
		return true;
	}
}

void init_tasks()
{
	num_tasks = 0;

	current_task_index = -1;
}

struct interrupt_registers* schedule_next_task(struct interrupt_registers* registers)
{
	if(num_tasks <= 0)
	{
		return registers;
	}

	//update current stopping task
	if(current_task_index >= 0)
	{
		tasks[current_task_index]->registers = registers;
	}
	
	//go to next task
	if(++current_task_index >= num_tasks)
	{
		current_task_index %= num_tasks;
	}

	//return next task
	return tasks[current_task_index]->registers;
}

