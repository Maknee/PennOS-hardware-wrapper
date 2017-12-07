#pragma once

#include "cpu.h"
#include "hal.h"
#include "includes.h"
#include "ports.h"
// Credits: BrokenThorn

/*
8259A Software Port Map
Port Address	Description
0x20	Primary PIC Command and Status Register
0x21	Primary PIC Interrupt Mask Register and Data Register
0xA0	Secondary (Slave) PIC Command and Status Register
0xA1	Secondary (Slave) PIC Interrupt Mask Register and Data Register
*/

/*
Initialization Control Word (ICW) 1
Bit Number	Value	Description
0	IC4	If set(1), the PIC expects to recieve IC4 during initialization.
1	SNGL	If set(1), only one PIC in system. If cleared, PIC is
cascaded with slave PICs, and ICW3 must be sent to controller.
2	ADI	If set (1), CALL address interval is 4, else 8. This is
useually ignored by x86, and is default to 0
3	LTIM	If set (1), Operate in Level Triggered Mode. If Not set
(0), Operate in Edge Triggered Mode
4	1	Initialization bit. Set 1 if PIC is to be initialized
5	0	MCS-80/85: Interrupt Vector Address. x86 Architecture: Must be 0
6	0	MCS-80/85: Interrupt Vector Address. x86 Architecture: Must be 0
7	0	MCS-80/85: Interrupt Vector Address. x86 Architecture: Must be 0
*/

#define PIC_ICW1_IC4 0x1 // 00000001
#define PIC_ICW1_SNGL 0x2 // 00000010
#define PIC_ICW1_ADI 0x4 // 00000100
#define PIC_ICW1_LTIM 0x8 // 00001000
#define PIC_ICW1_INIT 0x10 // 00010000

/*
Initialization Control Word (ICW) 2
Bit Number	Value	Description
0-2	A8/A9/A10	Address bits A8-A10 for IVT when in MCS-80/85 mode.
3-7	A11(T3)/A12(T4)/A13(T5)/A14(T6)/A15(T7)	Address bits A11-A15
for IVT when in MCS-80/85 mode. In 80x86 mode, specifies the interrupt vector
address. May be set to 0 in x86 mode.

Initialization Control Word (ICW) 3 - Primary PIC
Bit Number	Value	Description
0-7	S0-S7	Specifies what Interrupt Request (IRQ) is connected to slave PIC

*/

/*
Initialization Control Word (ICW) 4
Bit Number	Value	Description
0	uPM	If set (1), it is in 80x86 mode. Cleared if MCS-80/86 mode
1	AEOI	If set, on the last interrupt acknowledge pulse, controller
automatically performs End of Interrupt (EOI) operation
2	M/S	Only use if BUF is set. If set (1), selects buffer master.
Cleared if buffer slave.
3	BUF	If set, controller operates in buffered mode
4	SFNM	Special Fully Nested Mode. Used in systems with a large
amount of cascaded controllers.
5-7	0	Reserved, must be 0
*/

#define PIC_ICW4_UPM 0x1 // 00000001
#define PIC_ICW4_AEOI 0x2 // 00000010
#define PIC_ICW4_MS 0x4 // 00000100
#define PIC_ICW4_BUF 0x8 // 00001000
#define PIC_ICW4_SFNM 0x10 // 00010000

//-----------------------------------------------
//	Command words are used to control the devices
//-----------------------------------------------

//! Command Word 2 bit masks. Use when sending commands
#define PIC_OCW2_L1 1 // 00000001
#define PIC_OCW2_L2 2 // 00000010
#define PIC_OCW2_L3 4 // 00000100
#define PIC_OCW2_EOI 0x20 // 00100000
#define PIC_OCW2_SL 0x40 // 01000000
#define PIC_OCW2_ROTATE 0x80 // 10000000

//! Command Word 3 bit masks. Use when sending commands
#define PIC_OCW3_RIS 1 // 00000001
#define PIC_OCW3_RIR 2 // 00000010
#define PIC_OCW3_MODE 4 // 00000100
#define PIC_OCW3_SMM 0x20 // 00100000
#define PIC_OCW3_ESMM 0x40 // 01000000
#define PIC_OCW3_D7 0x80 // 10000000

//-----------------------------------------------
//	Controller Registers
//-----------------------------------------------

//! PIC 1 register port addresses
#define PIC1_REG_COMMAND 0x20
#define PIC1_REG_STATUS 0x20
#define PIC1_REG_DATA 0x21
#define PIC1_REG_IMR 0x21

//! PIC 2 register port addresses
#define PIC2_REG_COMMAND 0xA0
#define PIC2_REG_STATUS 0xA0
#define PIC2_REG_DATA 0xA1
#define PIC2_REG_IMR 0xA1

//-----------------------------------------------
//	Initialization Command Bit Masks
//-----------------------------------------------

//! The following devices use PIC 1 to generate interrupts
#define PIC_IRQ_TIMER 0
#define PIC_IRQ_KEYBOARD 1
#define PIC_IRQ_SERIAL2 3
#define PIC_IRQ_SERIAL1 4
#define PIC_IRQ_PARALLEL2 5
#define PIC_IRQ_DISKETTE 6
#define PIC_IRQ_PARALLEL1 7

//! The following devices use PIC 2 to generate interrupts
#define PIC_IRQ_CMOSTIMER 0
#define PIC_IRQ_CGARETRACE 1
#define PIC_IRQ_AUXILIARY 4
#define PIC_IRQ_FPU 5
#define PIC_IRQ_HDC 6

#define MASTER_PIC 1
#define SLAVE_PIC 2

//! Remap master and slave to 0x20 - 0x2f
#define PIC_MASTER_ISR 0x20
#define PIC_SLAVE_ISR 0x28

//! Read data byte from pic
uint8_t pic_read_data(uint8_t pic);

//! Send a data byte to pic
void pic_send_data(uint8_t pic, uint8_t data);

//! Send operational command to pic
void pic_send_command(uint8_t pic, uint8_t command);

//! Enables and disables interrupts
void pic_mask_irq(uint8_t pic, uint8_t irq);
void pic_unmask_irq(uint8_t pic, uint8_t irq);

//! Remap pic
void pic_remap(uint8_t master_isr, uint8_t slave_isr);

//! Initialize pic
void pic_init();
