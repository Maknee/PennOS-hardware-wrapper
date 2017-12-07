#pragma once

#include "includes.h"
#include "debug.h"
#include "stdio.h"
#include "malloc.h" //memory allocation functions

void itoa_u(uint32_t i, char* buf, uint32_t base);

void itoa(int32_t i, char* buf, uint32_t base);

int atoi(const char* str);

void exit(int status);


