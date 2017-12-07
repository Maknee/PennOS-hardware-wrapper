#pragma once

#include "cpu.h"
#include "idt.h"
#include "includes.h"
#include "isr.h"
#include "isr.h"
#include "pic.h"
#include "pit.h"
#include "ports.h"

//! initialize hardware abstraction layer
int hal_init();

//! shutdown hardware abstraction layer
int hal_shutdown();

//! generates interrupt
void generate_interrupt(int n);

//! output sound to speaker
void sound(uint32_t frequency);

//! enables all hardware interrupts
void enable_interrupts();

//! disable all hardware interrupts
void disable_interrupts();

//! returns cpu vender
const char* get_cpu_vender();

//! returns current tick count (only for demo)
uint32_t get_tick_count();
