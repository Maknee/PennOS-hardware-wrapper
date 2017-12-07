#pragma once

#include "includes.h"
#include "vga.h"

/*
x86 Interrupt Vector Table (IVT)
Base Address	Interrupt Number	Description
0x000	0	Divide by 0
0x004	1	Single step (Debugger)
0x008	2	Non Maskable Interrupt (NMI) Pin
0x00C	3	Breakpoint (Debugger)
0x010	4	Overflow
0x014	5	Bounds check
0x018	6	Undefined Operation Code (OPCode) instruction
0x01C	7	No coprocessor
0x020	8	Double Fault
0x024	9	Coprocessor Segment Overrun
0x028	10	Invalid Task State Segment (TSS)
0x02C	11	Segment Not Present
0x030	12	Stack Segment Overrun
0x034	13	General Protection Fault (GPF)
0x038	14	Page Fault
0x03C	15	Unassigned
0x040	16	Coprocessor error
0x044	17	Alignment Check (486+ Only)
0x048	18	Machine Check (Pentium/586+ Only)
0x05C	19-31	Reserved exceptions
0x068 - 0x3FF	32-255	Interrupts free for software use
*/

/*
x86 Hardware Interrupts
8259A Input pin	Interrupt Number	Description
IRQ0    	0x08	Timer
IRQ1 	        0x09	Keyboard
IRQ2    	0x0A	Cascade for 8259A Slave controller
IRQ3		0x0B	Serial port 2
IRQ4		0x0C	Serial port 1
IRQ5		0x0D	AT systems: Parallel Port 2. PS/2 systems: reserved
IRQ6		0x0E	Diskette drive
IRQ7		0x0F	Parallel Port 1
IRQ8/IRQ0	0x70	CMOS Real time clock
IRQ9/IRQ1	0x71	CGA vertical retrace
IRQ10/IRQ2	0x72	Reserved
IRQ11/IRQ3	0x73	Reserved
IRQ12/IRQ4	0x74	AT systems: reserved. PS/2: auxiliary device
IRQ13/IRQ5	0x75	FPU
IRQ14/IRQ6	0x76	Hard disk controller
IRQ15/IRQ7	0x77	Reservedi
*/

/*
The interrupt is generated

When a device controller needs to generate an interrupt, it needs to signal the
PIC somehow. Lets say, for purposes of discussion, that this device is the
timer, which uses interrupt line 0.
The timer controller signals the PIC by activating the IR0 line. This changes
its state from a 0 (No power) to a 1 (Power is going through the line.)
The PIC sets the bit representing the IRQ inside of the Interrupt Request
Register (IRR). In this example, bit 0 will be set to 1.
The PIC examins the Interrupt Mask Register (IMR) to see if the interrupt can be
serviced.
If the interrupt can be serviced, the PIC determins if there are any higher
priority interrupts waiting to be serviced. If there is, the interrupt request
is ignored until the higher priority interrupts are serviced.
If the interrupt can be serviced, and there are no higher priority interrupts,
the PIC continues to the next step.
The PIC signals the processor through the INTA pin to inform the processor an
interrupt has been fired.
The processor now knows that an interrupt has been fired.

The processor acnowledges the interrupt

The CPU completes execution of the current instruction.
The CPU examins the Interrupt Flag (IF) within RFLAGS.
If IF is set, the CPU acnowledges the interrupt request through the INTR pin
back to the PIC.
If IF is cleared, the interrupt request is ignored.
The PIC recieves the acnowledgment signal through INTR.
The PIC places the interrupt vector number into the D0-D7 pins.
This interrupt vector number is abtained from the Initialization Control Word
(ICW) 2 during initialization of the PIC. We will cover this later.
The PIC also places the IRQ number into D0-D7
The PIC sets the correct bit inside of the In Service Register (ISR). In this
case, it is bit 0. This indicates that Interrupt 0 is currently being serviced.

Interruption

The processor interrupts the current process. It pushes EFLAGS, CS, and EIP on
the stack.
The processor uses the interrupt vector number (given by the PIC).
In real mode, the CPU offsets into the IVT. In Protected Mode, The Processor
offsets into the IDT.
Real Mode:
The CPU offsets into the correct entry into the IVT
The CPU loads the base address of the interrupt to call into CS:IP
The interrupt takes control.
Protected Mode:
The CPU uses the loaded IDT to offset into
The selecter feild of the gate descriptor is loaded into the CS segment
selector.
The offset feild of the gate descriptor is loaded into EIP.
If paging is enabled, this address is translated from a linear address to a
physical address.
Now, the CPU will perform architecture specific security checks on the current
state.
The interrupt routine can now take control from gate descriptor + CS:EIP.
*/

#define IDT_16BITS 0x06 // 00000110
#define IDT_32BITS 0x0E // 00001110
#define IDT_RING1 0x40 // 01000000
#define IDT_RING2 0x20 // 00100000
#define IDT_RING3 0x60 // 01100000
#define IDT_PRESENT 0x80 // 10000000

#define MAX_INTERRUPTS 256

typedef struct
{
    uint16_t base_low; // lower 16 bits of handler's address
    uint16_t selector; // segment selector -> gdt
    uint8_t reserved; // reserved
    uint8_t flags; // Should be 010001110 first 5 = interrupt gate
    uint16_t base_high; // higher 16 bits of handler's address
} __attribute__((packed)) idt_entry;

typedef struct
{
    uint16_t limit; // size of idt table
    uint32_t base; // base address of idt
} __attribute__((packed)) idt_register;

// ! loads the idt into idtr
void load_idt();

// ! returns pointer to descriptor
idt_entry* get_idt_entry(uint32_t index);

// ! installs idt entry into descriptor table
void install_idt_entry(uint32_t index, uint8_t flags, uint16_t selector,
    uint32_t handler_address);

// ! setups the idt
void idt_init();
