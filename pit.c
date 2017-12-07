#include "pit.h"

// global tick counter
static volatile uint32_t tick_count = 0;

// current sigaction for timer
struct sigaction pit_sigaction = {.sa_flags = SA_INVALID};

// is sigaction blocked?
bool pit_sigaction_blocked;

// itimerval
struct itimerval pit_itimerval = { 0 };

//assembly stub for pit sigaction
extern void pit_sigaction_handler_stub();

//keep track of previous eip after jump from interrupt for timer sigaction
uint32_t pit_sigaction_prev_eip;

extern struct interrupt_registers* cpu_registers;

// interrupt handler for pit
void pit_irq(interrupt_registers* registers)
{
    // increment count
    tick_count++;

	//tv_usec is the most important one
	if(pit_itimerval.it_interval.tv_usec && !pit_sigaction_blocked)
	{
	//calculate if time is ready (not exactly right since tick count is the ticks, not the actual time
	
	//should be this, but I want fast response time
	//if(!(tick_count % (pit_itimerval.it_interval.tv_usec / 1000)))
	
	if(!(tick_count % (pit_itimerval.it_interval.tv_usec / 100000)))
	{
	//check if we need to handle pit sigaction
	if(!(pit_sigaction.sa_flags & SA_INVALID))
	{
		pit_sigaction_handler();
		
		//no idea why this works
		/*
		if(registers->eip != (uint32_t)pit_sigaction_handler_stub)
		{
			pit_sigaction_prev_eip = registers->eip;
			registers->eip = (uint32_t)pit_sigaction_handler_stub;
		}
		*/
	}
	}
	}
}

//gets called when sigaction exists
uint32_t pit_sigaction_handler()
{
	//check if valid sigaction
	if(!(pit_sigaction.sa_flags & SA_INVALID))
	{
		//check if needed to be resetted
		if(!(pit_sigaction.sa_flags & SA_RESTART))
		{
			//set to invalid again
			pit_sigaction.sa_flags &= ~(SA_RESTART);
		}

		if(pit_sigaction.sa_flags & SA_SIGINFO)
		{
			//call extended function (pass nulls, cause unused)
			if(pit_sigaction.sa_sigaction != NULL)
			{
				pit_sigaction.sa_sigaction(SIGALRM, NULL, NULL);
			}
		}
		else
		{
			//call normal function
			if(pit_sigaction.sa_handler != NULL)
			{
				pit_sigaction.sa_handler(SIGALRM);
			}
		}
	}
	return pit_sigaction_prev_eip;
}

// send byte to command port
void pit_send_command(uint8_t cmd)
{
    port_write_byte(PIT_REG_COMMAND, cmd);
}

// send data to counter data port
void pit_send_data(uint8_t data, uint8_t counter)
{
    uint8_t port;
    if (counter == PIT_REG_COUNTER_0)
    {
        port = PIT_REG_COUNTER_0;
    }
    else if (counter == PIT_REG_COUNTER_1)
    {
        port = PIT_REG_COUNTER_1;
    }
    else
    {
        port = PIT_REG_COUNTER_2;
    }
    port_write_byte(port, data);
}

uint8_t pit_read_data(uint16_t counter)
{
    uint8_t port;
    if (counter == PIT_REG_COUNTER_0)
    {
        port = PIT_REG_COUNTER_0;
    }
    else if (counter == PIT_REG_COUNTER_1)
    {
        port = PIT_REG_COUNTER_1;
    }
    else
    {
        port = PIT_REG_COUNTER_2;
    }
    return port_read_word(port);
}

uint32_t pit_set_tick_count(uint32_t new_tick)
{
    uint32_t prev_tick_count = tick_count;
    tick_count               = new_tick;
    return prev_tick_count;
}

uint32_t pit_get_tick_count()
{
    return tick_count;
}

void pit_configure(uint16_t freq, uint8_t counter, uint8_t mode)
{
    if (freq == 0)
    {
        return;
    }

    uint16_t divisor = (uint16_t)(TIMER_FREQUENCY / freq);

    // set operation command
    pit_send_command(mode | PIT_OCW_READ_LSB_MSB | counter);

    // set frequency
    pit_send_data((uint8_t)(divisor & 0xFF), PIT_REG_COUNTER_0);
    pit_send_data((uint8_t)((divisor >> 8) & 0xFF), PIT_REG_COUNTER_0);

    // reset tick count
    tick_count = 0;
}

void pit_init()
{
    // install interrupt handler (IRQ 0 maps to => interrupt 32)
    register_interrupt_handler(IRQ0, pit_irq);

    pit_configure(TIMER_TICKS, PIT_OCW_COUNTER_0, PIT_OCW_SQUARE_WAVE_GENERATOR);
}
