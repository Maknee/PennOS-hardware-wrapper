#pragma once

#include "gdt.h"
#include "idt.h"
#include "includes.h"

#define CODE_DESCRIPTOR 0x8

void cpu_init();
void cpu_shutdown();
char* cpu_get_vender();
