#pragma once

#include "hal.h"
#include "includes.h"
#include "isr.h"
#include "ports.h"
#include "signal.h"
#include "task.h"
#include "time.h"

/*
PIT OCW
Bit 0: (BCP) Binary Counter
0: Binary
1: Binary Coded Decimal (BCD)
Bit 1-3: (M0, M1, M2) Operating Mode. See above sections for a description of
each.
000: Mode 0: Interrupt or Terminal Count
001: Mode 1: Programmable one-shot
010: Mode 2: Rate Generator
011: Mode 3: Square Wave Generator
100: Mode 4: Software Triggered Strobe
101: Mode 5: Hardware Triggered Strobe
110: Undefined; Don't use
111: Undefined; Don't use
Bits 4-5: (RL0, RL1) Read/Load Mode. We are going to read or send data to a
counter register
00: Counter value is latched into an internal control register at the time of
the I/O write operation.
01: Read or Load Least Significant Byte (LSB) only
10: Read or Load Most Significant Byte (MSB) only
11: Read or Load LSB first then MSB
Bits 6-7: (SC0-SC1) Select Counter. See above sections for a description of
each.
00: Counter 0
01: Counter 1
10: Counter 2
11: Illegal value
*/

#define PIT_OCW_BINARY_CODED_DECIMAL 0x1 // 00000001
#define PIT_OCW_TERMINAL_COUNT 0x0 // 00000000
#define PIT_OCW_ONE_SHOT 0x2 // 00000010
#define PIT_OCW_RATE_GENERATOR 0x4 // 00000100
#define PIT_OCW_SQUARE_WAVE_GENERATOR 0x6 // 00000110
#define PIT_OCW_SOFTWARE_STROBE 0x8 // 00001000
#define PIT_OCW_HARDWARE_STROBE 0xA // 00001010
#define PIT_OCW_READ_LSB 0x10 // 00010000
#define PIT_OCW_READ_MSB 0x20 // 00100000
#define PIT_OCW_READ_LSB_MSB 0x30 // 0011000
#define PIT_OCW_COUNTER_0 0x0 // 00000000
#define PIT_OCW_COUNTER_1 0x40 // 01000000
#define PIT_OCW_COUNTER_2 0x80 // 10000000

// ports
#define PIT_REG_COUNTER_0 0x40 // data port for counter 0
#define PIT_REG_COUNTER_1 0x41 // data port for counter 1
#define PIT_REG_COUNTER_2 0x42 // data port for counter 2
#define PIT_REG_COMMAND 0x43 // command port for pit

#define TIMER_FREQUENCY 1193180

#define TIMER_TICKS 100

//handle sigaction if there exists one
uint32_t pit_sigaction_handler();

void pit_send_command(uint8_t cmd);
void pit_send_data(uint8_t data, uint8_t counter);
uint8_t pit_read_data(uint16_t counter);
uint32_t pit_set_tick_count(uint32_t new_tick);
uint32_t pit_get_tick_count();
void pit_configure(uint16_t freq, uint8_t counter, uint8_t mode);
void pit_init();
