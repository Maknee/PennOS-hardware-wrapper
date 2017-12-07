#include "isr.h"

//array of function pointers to handle each interrupt
static isr_handler isr_handlers[MAX_INTERRUPTS];

//! sets new interrupt vector
void set_idt_gate(uint8_t index, uint32_t new_handler)
{
    //! install interrupt handler! This overwrites prev interrupt descriptor
    install_idt_entry(index, IDT_PRESENT | IDT_32BITS, GDT_CODE_SEGMENT, new_handler);
}

//! returns current interrupt vector
uint32_t get_idt_gate(uint8_t index)
{
    //! get the descriptor from the idt
    idt_entry* entry = get_idt_entry(index);

    if (!entry)
    {
        return 0;
    }

    //! get address of interrupt handler
    uint32_t address = entry->base_low | (entry->base_high << 16);

    //! return interrupt handler
    return address;
}

/* To print the message which defines every exception */
static const char* exception_messages[] = { "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved" };

static void default_isr_handler(interrupt_registers* registers)
{
	uint8_t x = vga_get_x();
	uint8_t y =vga_get_y();
    vga_go_to_pixel(0, 0);
    vga_printf("Got interrupt %d exception: ", registers->int_no);
    vga_printf("%s", exception_messages[registers->int_no]);
    vga_go_to_pixel(x, y);
}

void isr_init()
{
    //Set the processor handlers
    set_idt_gate(0, (uint32_t)isr0);
    set_idt_gate(1, (uint32_t)isr1);
    set_idt_gate(2, (uint32_t)isr2);
    set_idt_gate(3, (uint32_t)isr3);
    set_idt_gate(4, (uint32_t)isr4);
    set_idt_gate(5, (uint32_t)isr5);
    set_idt_gate(6, (uint32_t)isr6);
    set_idt_gate(7, (uint32_t)isr7);
    set_idt_gate(8, (uint32_t)isr8);
    set_idt_gate(9, (uint32_t)isr9);
    set_idt_gate(10, (uint32_t)isr10);
    set_idt_gate(11, (uint32_t)isr11);
    set_idt_gate(12, (uint32_t)isr12);
    set_idt_gate(13, (uint32_t)isr13);
    set_idt_gate(14, (uint32_t)isr14);
    set_idt_gate(15, (uint32_t)isr15);
    set_idt_gate(16, (uint32_t)isr16);
    set_idt_gate(17, (uint32_t)isr17);
    set_idt_gate(18, (uint32_t)isr18);
    set_idt_gate(19, (uint32_t)isr19);
    set_idt_gate(20, (uint32_t)isr20);
    set_idt_gate(21, (uint32_t)isr21);
    set_idt_gate(22, (uint32_t)isr22);
    set_idt_gate(23, (uint32_t)isr23);
    set_idt_gate(24, (uint32_t)isr24);
    set_idt_gate(25, (uint32_t)isr25);
    set_idt_gate(26, (uint32_t)isr26);
    set_idt_gate(27, (uint32_t)isr27);
    set_idt_gate(28, (uint32_t)isr28);
    set_idt_gate(29, (uint32_t)isr29);
    set_idt_gate(30, (uint32_t)isr30);
    set_idt_gate(31, (uint32_t)isr31);

    // Install the IRQs
    set_idt_gate(32, (uint32_t)irq0);
    set_idt_gate(33, (uint32_t)irq1);
    set_idt_gate(34, (uint32_t)irq2);
    set_idt_gate(35, (uint32_t)irq3);
    set_idt_gate(36, (uint32_t)irq4);
    set_idt_gate(37, (uint32_t)irq5);
    set_idt_gate(38, (uint32_t)irq6);
    set_idt_gate(39, (uint32_t)irq7);
    set_idt_gate(40, (uint32_t)irq8);
    set_idt_gate(41, (uint32_t)irq9);
    set_idt_gate(42, (uint32_t)irq10);
    set_idt_gate(43, (uint32_t)irq11);
    set_idt_gate(44, (uint32_t)irq12);
    set_idt_gate(45, (uint32_t)irq13);
    set_idt_gate(46, (uint32_t)irq14);
    set_idt_gate(47, (uint32_t)irq15);

	//default every exception
	for(uint8_t i = 0; i < 47; i++)
	{
		//skip timer and keyboard
		if(i == IRQ0 || i == IRQ1)
		{
			continue;
		}

		register_interrupt_handler(i, default_isr_handler);
	}
}

void register_interrupt_handler(uint8_t index, isr_handler handler)
{
    isr_handlers[index] = handler;
}

uint32_t irq_wrapper_handler(interrupt_registers* registers)
{
    /* Handle the interrupt in a more modular way */
 	/*
 	if (isr_handlers[registers->int_no] != 0)
    {
        //get the indexed handler
        isr_handler handler = isr_handlers[registers->int_no];

        //call the index handler
        handler(registers);
    }
    */

    // if idt entry invoked is >= 40 (IRQ8 - 15), then send EOI to slave
    if (registers->int_no >= 40)
    {
        pic_send_command(SLAVE_PIC, PIC_OCW2_EOI);
    }

    //Always send EOI to master
    pic_send_command(MASTER_PIC, PIC_OCW2_EOI);
	
	//pit go to next task if possible
	if(registers->int_no == IRQ0)
	{
		registers = schedule_next_task(registers);
	}
 	
	if (isr_handlers[registers->int_no] != 0)
    {
        //get the indexed handler
        isr_handler handler = isr_handlers[registers->int_no];

        //call the index handler
        handler(registers);
    }
    
    return (uint32_t)registers;
}

uint32_t isr_wrapper_handler(interrupt_registers* registers)
{
    /* Handle the interrupt in a more modular way */
    if (isr_handlers[registers->int_no] != 0)
    {
        //get the indexed handler
        isr_handler handler = isr_handlers[registers->int_no];

        //call the index handler
        handler(registers);
    }

    return (uint32_t)registers;
}

