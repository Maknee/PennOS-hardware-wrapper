#include "debug.h"

void abort()
{
	vga_printf("OS ABORTED. SOMETHING WENT WRONG");
    __asm__ __volatile__("cli");
    __asm__ __volatile__("hlt");
}
