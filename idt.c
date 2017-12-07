#include "idt.h"
#include "hal.h"

//array of idt entries in table
static idt_entry idt[MAX_INTERRUPTS]; // 256 interrupts

//register for idtr
static idt_register idtr; // one idt register

void load_idt() // loads the idt into idtr
{
    __asm__ __volatile__("lidtl (%0)"
                         :
                         : "r"(&idtr));
}

void default_handler() // default handler for idts
{
    vga_clear(' ');
    vga_go_to_pixel(0, 0);
    vga_printf("DEFAULT EXCEPTION HANDLER");
    while (1)
    {
    }
}

idt_entry* get_idt_entry(uint32_t index) // get pointer to descriptor
{
    if (index > MAX_INTERRUPTS)
    {
        return 0;
    }

    return &idt[index];
}

void install_idt_entry(
    uint32_t index, uint8_t flags, uint16_t selector,
    uint32_t handler_address) // installs an ir into the descriptor table
{
    if (index > MAX_INTERRUPTS)
    {
        return;
    }

    if (handler_address == 0)
    {
        return;
    }

    idt[index].base_low  = (uint16_t)(handler_address & 0xFFFF);
    idt[index].base_high = (uint16_t)((handler_address >> 16) & 0xFFFF);
    idt[index].reserved  = 0;
    idt[index].flags     = flags;
    idt[index].selector  = selector;
}

void idt_init() // setups the descriptor table and the interrupt routines
{
    // setup idtr
    idtr.limit = (sizeof(idt_entry) * MAX_INTERRUPTS) - 1;
    idtr.base  = (uint32_t)&idt[0];

    // zero out idt

    for (uint32_t i = 0; i < MAX_INTERRUPTS; i++)
    {
        // using compound literals to zero intialize
        idt[i] = (idt_entry){ 0 };
    }

    // load idt
    load_idt();
}
