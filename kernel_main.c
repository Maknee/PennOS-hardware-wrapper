#include "debug.h"
#include "hal.h"
#include "includes.h"
#include "keyboard.h"
#include "us_keyboard.h"
#include "multiboot.h"
#include "phys_mm.h"
#include "vga.h"
#include "virtual_mm.h"
#include "signal.h"
#include "ucontext.h"
#include "malloc.h"

#define SCREEN_COLOR 0x2

void sleep(uint32_t ms)
{
    uint32_t end = ms + pit_get_tick_count();
    while (pit_get_tick_count() < end)
    {
        ;
    }
}

void draw_turtles()
{
    vga_printf("   /----\\\n");
    vga_printf(" --------\\\n");
    vga_printf("   -------\\\n");
}

void check_for_commands(const char* buffer)
{
    if (!strcmp(buffer, "clear"))
    {
        vga_clear(' ');
        vga_go_to_pixel(0, 0);
    }
    else if (!strcmp(buffer, "I like turtles"))
    {
        draw_turtles();
    }
    else if (!strcmp(buffer, "help"))
    {
        vga_printf("Help?\n");
    }
    else
    {
        vga_printf("Invalid command\n");
    }
}

ucontext_t con;
ucontext_t con2;
ucontext_t con3;
uint8_t ucontext_stack[SIGSTKSZ];
uint8_t ucontext_stack2[SIGSTKSZ];
uint8_t ucontext_stack3[SIGSTKSZ];

void print_something(int one, int two)
{
	vga_printf("AAAAAAAAAAA %d %d\n", one, two);
}

void swap()
{
	while(1)
	{
		printf("HI\n");
		swapcontext(&con3, &con);
	}
}

void k_shell()
{
    draw_turtles();
    while (1)
    {
        char buffer[100] = { 0 };
        keyboard_read_buffer(buffer, sizeof(buffer) - 1);
        vga_printf("%s", buffer);
	swapcontext(&con, &con3);
    }
}

void k_timer_handler(int signo, siginfo_t* si, void* old_context)
{
}

void k_signal_handler(int signal)
{
	if(signal == SIGINT)
	{
		vga_printf("SIGINT\n");
	}
	else if (signal == SIGTSTP)
	{
		vga_printf("SIGTSTP\n");
	}
}

extern memory_info mem_info;

int kernel_main(uint32_t boot_magic, multiboot_info_t* boot_info)
{
    if (boot_magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        vga_printf("Bootloader MAGIC is invalid... Halting\n");
        return -1;
    }

    disable_interrupts();

    vga_set_color(SCREEN_COLOR);
    vga_clear(' ');
    vga_go_to_pixel(0, 0);

    gdt_init();
    vga_printf("GDT initailized\n");

    idt_init();
    vga_printf("IDT initailized\n");

    isr_init();
    vga_printf("ISR initialized\n");

    pic_init();
    vga_printf("PIC initialized\n");

    pit_init();
    vga_printf("PIT initialized\n");

    phys_mm_init(boot_info);
    vga_printf("Physical memory manager initialized\n");

    keyboard_init(&keyboard_us_qwerty);
    vga_printf("Keyboard initialized\n");

	//pretty brute force of doing this =/
	uint32_t malloc_size = 0x4000000;
	uint32_t malloc_start = mem_info.heap_start;
	phys_mm_set_blocks(malloc_start, malloc_size);

	//MAKE SURE THIS IS 0'd out
	memset(malloc_start, 0, malloc_size);
    init_mm(malloc_start, malloc_size);
    vga_printf("Memory manager initialized\n");
	

	//allocate more, to make sure nothing bad happens?
	void* fs_addr = calloc(1024, 4096);
	init_unistd_lib(fs_addr);
	vga_printf("Unistd lib initialized\n");
	
	vga_printf("Memory map of system\n");
	phys_mm_debug();
   
    /*
	struct sigaction sa;
	sa.sa_handler = NULL;
	sa.sa_sigaction = &k_timer_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART | SA_SIGINFO;
	if(sigaction(SIGALRM, &sa, NULL))
	{
		ABORT_MSG("sigalrm could not be installed\n");
	}

	sa.sa_sigaction = NULL;
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = &k_signal_handler;

	if(sigaction(SIGINT, &sa, NULL))
	{
		ABORT_MSG("sigint could not be installed\n");
	}
	if(sigaction(SIGTSTP, &sa, NULL))
	{
		ABORT_MSG("sigtstp could not be installed\n");
	}
	*/

//	init_tasks();
//	task_t task1;
//	install_task(&task1, ya);

/*
	getcontext(&con);
	
	con.uc_link = NULL;
	
	con.uc_stack.ss_sp = ucontext_stack;
	con.uc_stack.ss_size = SIGSTKSZ;
	con.uc_stack.ss_flags = 0;
	
	makecontext(&con, k_shell, 0);
*/
/*
	getcontext(&con2);
	
	con2.uc_link = &con;
	
	con2.uc_stack.ss_sp = ucontext_stack2;
	con2.uc_stack.ss_size = SIGSTKSZ;
	con2.uc_stack.ss_flags = 0;
	
	makecontext(&con2, print_something, 2, 1, 2);
    
	setcontext(&con2);
*/
/*
	getcontext(&con3);
	
	con3.uc_link = NULL;
	
	con3.uc_stack.ss_sp = ucontext_stack3;
	con3.uc_stack.ss_size = SIGSTKSZ;
	con3.uc_stack.ss_flags = 0;
	
	makecontext(&con3, swap, 0);
	
	setcontext(&con3);
*/

    enable_interrupts();
	
	//make_flat();

	extern void main(int argc, char* argv[]);

	//main(2, (char**)312312);

	vga_printf("Please insert main function above\n");

    return 0;
}
