#include "keyboard.h"

// sigint/sigstsp
struct sigaction sigint_sigaction = {.sa_flags = SA_INVALID};
struct sigaction sigtstp_sigaction = {.sa_flags = SA_INVALID};

// is sigaction blocked?
bool sigint_sigaction_blocked;
bool sigtstp_sigaction_blocked;

// assembly strub for sigactions
extern void sigint_sigaction_handler_stub();
extern void sigtstp_sigaction_handler_stub();

// prev eips
uint32_t sigint_sigaction_prev_eip;
uint32_t sigtstp_sigaction_prev_eip;

// currently selected keyboard driver
static keyboard_driver* current_keyboard_driver;

// lock keys
static bool numlock_key, scroll_lock_key, caps_lock_key;

// combination keys
static bool shift_key, alt_key, l_control_key;

// error
static uint32_t keyboard_error = 0;

// keyboard buffer
static char* keyboard_buffer;

// keyboard buffer index
static uint32_t keyboard_buffer_index;

// limit to the size of buffer
static uint32_t keyboard_buffer_limit;

// status of keyboard buffer
static bool buffer_ready_status;

//gets called when sigaction exists
uint32_t sigint_sigaction_handler()
{
	//check if valid sigaction
	if(!(sigint_sigaction.sa_flags & SA_INVALID))
	{
		//check if needed to be resetted
		if(!(sigint_sigaction.sa_flags & SA_RESTART))
		{
			//set to invalid again
			sigint_sigaction.sa_flags &= ~(SA_RESTART);
		}

		if(sigint_sigaction.sa_flags & SA_SIGINFO)
		{
			//call extended function (pass nulls, cause unused)
			if(sigint_sigaction.sa_sigaction != NULL)
			{
				sigint_sigaction.sa_sigaction(SIGINT, NULL, NULL);
			}
		}
		else
		{
			//call normal function
			if(sigint_sigaction.sa_handler != NULL)
			{
				sigint_sigaction.sa_handler(SIGINT);
			}
		}
	}
	return sigint_sigaction_prev_eip;
}


//gets called when sigaction exists
uint32_t sigtstp_sigaction_handler()
{
	//check if valid sigaction
	if(!(sigtstp_sigaction.sa_flags & SA_INVALID))
	{
		//check if needed to be resetted
		if(!(sigtstp_sigaction.sa_flags & SA_RESTART))
		{
			//set to invalid again
			sigtstp_sigaction.sa_flags &= ~(SA_RESTART);
		}

		if(sigtstp_sigaction.sa_flags & SA_SIGINFO)
		{
			//call extended function (pass nulls, cause unused)
			if(sigtstp_sigaction.sa_sigaction != NULL)
			{
				sigtstp_sigaction.sa_sigaction(SIGTSTP, NULL, NULL);
			}
		}
		else
		{
			//call normal function
			if(sigtstp_sigaction.sa_handler != NULL)
			{
				sigtstp_sigaction.sa_handler(SIGTSTP);
			}
		}
	}
	return sigtstp_sigaction_prev_eip;
}

// check if scancode exists
static bool contains_scancode(keyboard_scancode_set* scancode_set, uint8_t scancode)
{
	for(uint32_t index = 0; index < scancode_set->length; index++)
	{
		if(scancode_set->scancodes[index] == scancode)
		{
			return true;
		}
	}
	return false;
}

static bool write_to_keyboard_buffer(char character, bool direction)
{
	if(keyboard_buffer != NULL && !buffer_ready_status)
	{
		if(direction && keyboard_buffer_index != keyboard_buffer_limit)
		{

			//NOT A CLUE WHY WHEN WRITING TO A FILE EOF WORKS, BUT END OF LINE DOESN'T??? like cat > 1 pressing enter ends read
			//if EOF
			if(l_control_key && character == 'd')
			{
				keyboard_buffer[keyboard_buffer_index++] = '\0';
				//tell the user that buffer is ready
				buffer_ready_status = true;
				
				//reset buffer
				keyboard_buffer = NULL;

				return false;
			}

			keyboard_buffer[keyboard_buffer_index++] = character;
			keyboard_buffer[keyboard_buffer_index] = '\0';

			if(character == '\n')
			{
				//tell the user that buffer is ready
				buffer_ready_status = true;
				
				//reset buffer
				keyboard_buffer = NULL;
			}
		}
		else if(!direction && keyboard_buffer_index != 0)
		{
			keyboard_buffer[keyboard_buffer_index--] = character;
		}
	}
	return true;
}

// interrupt handler for keyboard
void keyboard_irq(__attribute__((unused)) interrupt_registers* registers)
{
    // read if keyboard controller output buffer is ready
    if (keyboard_controller_read_status() & KEYBOARD_CONTROLLER_STATUS_OUT_BUF)
    {
        // read scan code from encoder
        uint8_t scancode = keyboard_encoder_read_input();

	//vga_printf("|%x|", scancode);

        // extended code... well, just don't parse it
        if (scancode == 0xE0 || scancode == 0xE1)
        {
		
        }
        // if the code is a break code
        else if (scancode & 0x80)
        {
		if(contains_scancode(current_keyboard_driver->keyboard_shift, (uint8_t)(scancode - 0x80)))
		{
			shift_key = false;
		}
		if(current_keyboard_driver->keyboard_l_ctrl_sc == (uint32_t)(scancode -0x80))
		{
			l_control_key = false;
		}
        }
        else
        {
		char character = '\0';
		if(contains_scancode(current_keyboard_driver->keyboard_shift, scancode))
		{
			shift_key = true;
			return;
		}
		
		if(current_keyboard_driver->keyboard_caps_sc == scancode)
		{
			caps_lock_key = !caps_lock_key;
			return;
		}

		if(current_keyboard_driver->keyboard_l_ctrl_sc == scancode)
		{
			l_control_key = true;
			return;
		}

		if(shift_key && caps_lock_key)
		{
			character = current_keyboard_driver->keyboard_sc_shift_capslock->scancodes[scancode];
		}
		else if(shift_key)
		{
			character = current_keyboard_driver->keyboard_sc_shift->scancodes[scancode];
		}
		else if(caps_lock_key)
		{
			character = current_keyboard_driver->keyboard_sc_capslock->scancodes[scancode];
		}
		else
		{
			character = current_keyboard_driver->keyboard_sc->scancodes[scancode];
		}

		//check if we need to handle signal handlers
		if(l_control_key && character == 'c')
		{
			if(!(sigint_sigaction.sa_flags & SA_INVALID) && !sigint_sigaction_blocked)
			{
			/*
			if(registers->eip != (uint32_t)sigint_sigaction_handler_stub)
			{
				sigint_sigaction_prev_eip = registers->eip;
				registers->eip = (uint32_t)sigint_sigaction_handler_stub;
				//print characters like in unix
				vga_printf("^C");
			}
			*/
			vga_printf("^C");
			sigint_sigaction_handler();
			}
		}
		else if(l_control_key && character == 'z')
		{
			if(!(sigtstp_sigaction.sa_flags & SA_INVALID) && !sigtstp_sigaction_blocked)
			{
			/*
			if(registers->eip != (uint32_t)sigtstp_sigaction_handler_stub)
			{
				sigtstp_sigaction_prev_eip = registers->eip;
				registers->eip = (uint32_t)sigtstp_sigaction_handler_stub;
				//print characters like in unix
				vga_printf("^Z");
			}
			*/
			vga_printf("^Z");
			sigtstp_sigaction_handler();
			}
		}
		//backspace
		else if(character == '\b')
		{
			if(write_to_keyboard_buffer('\0', false))
			{
				vga_backspace();
			}
		}
		//print key
		else
		{
			if(write_to_keyboard_buffer(character, true))
			{
				vga_putc(character);
			}
		}
        }
        switch (scancode)
        {
        case KEYBOARD_BAT_FAILED:
            keyboard_error = KEYBOARD_BAT_FAILED;
            break;
        case KEYBOARD_DIAG_FAILED:
            keyboard_error = KEYBOARD_DIAG_FAILED;
            break;
        case KEYBOARD_RESEND_COMMAND:
            keyboard_error = KEYBOARD_RESEND_COMMAND;
            break;
        }
    }
}

bool keyboard_read_buffer(char* buffer, uint32_t size)
{
	//set buffer to start reading into buffer
	keyboard_buffer = buffer;
	
	//reset buffer to beginning
	keyboard_buffer_index = 0;

	//set the limit
	keyboard_buffer_limit = size;
	
	//reset status
	buffer_ready_status = false;
	
	//wait until buffer is ready
	while(!buffer_ready_status);

	//read buffer
	return true;
}

// read data from keyboard controller
uint8_t keyboard_encoder_read_input()
{
    return port_read_byte(KEYBOARD_ENCODER_INPUT_BUFFER);
}

// send byte to command port
void keyboard_encoder_send_command(uint8_t cmd)
{
    while (true)
    {
        if ((keyboard_controller_read_status() & KEYBOARD_CONTROLLER_STATUS_IN_BUF) == 0)
        {
            break;
        }
    }
    port_write_byte(KEYBOARD_ENCODER_COMMAND_REGISTER, cmd);
}

// read status of keyboard controller
uint8_t keyboard_controller_read_status()
{
    return port_read_byte(KEYBOARD_CONTROLLER_STATUS_REGISTER);
}

// send byte to command port
void keyboard_controller_send_command(uint8_t cmd)
{
    while (true)
    {
        if ((keyboard_controller_read_status() & KEYBOARD_CONTROLLER_STATUS_IN_BUF) == 0)
        {
            break;
        }
    }
    port_write_byte(KEYBOARD_CONTROLLER_COMMAND_REGISTER, cmd);
}

void keyboard_init(keyboard_driver* driver)
{
	//remove any keys that we have pressed before setting up over interrupt handler
	while((keyboard_controller_read_status() & KEYBOARD_CONTROLLER_STATUS_OUT_BUF) == 1)
	{
		keyboard_encoder_read_input();
	}

	//enable keyboard contoller
	keyboard_controller_send_command(0xAE);

	//send command to get current state of keyboard controller
	keyboard_controller_send_command(0x20);
	
	//get current state
	uint8_t status = (keyboard_encoder_read_input() | 1) & (~0x10);

	//send command to change current state
	keyboard_controller_send_command(0x60);

	//send status to change current state
	keyboard_encoder_send_command(status);

	//enable keyboard encoder
	//keyboard_encoder_send_command(0xF4);

	//set current keyboard driver
    current_keyboard_driver = driver;

    // install interrupt handler (IRQ 1 maps to => interrupt 33)
    register_interrupt_handler(IRQ1, keyboard_irq);
}
