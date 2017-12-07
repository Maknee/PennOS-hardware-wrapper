#include "cpu.h"

void cpu_init()
{
    gdt_init();
    idt_init(CODE_DESCRIPTOR);
}

void cpu_shutdown()
{
}

char* cpu_get_vender()
{
    static char vender[32] = { 0 };
    __asm__ __volatile__("movl $0, %%eax;\n"
                         "cpuid;\n"
                         "leal (%0), %%eax;\n"
                         "movl %%ebx, (%%eax);\n"
                         "movl %%edx, 0x4(%%eax);\n"
                         "movl %%ecx, 0x8(%%eax)"
                         : "=m"(vender)
                         :
                         :);
    return vender;
}
