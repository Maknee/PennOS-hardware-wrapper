#pragma once

#include "vga.h"
#include "stdarg.h"
#include "ctype.h"

//define printf
#define printf(...) vga_printf(__VA_ARGS__)

//fprintf does printf
#define fprintf(x, ...) vga_printf(__VA_ARGS__)

//fopen does nothing
#define fopen(...) NULL
#define fflush(...)
#define fclose(...)

//perror does printf 
#define perror(...) vga_printf(__VA_ARGS__)

//FILE* is nothing
typedef uint32_t FILE;

//broken implemenation
int snprintf(char * s, size_t n, const char * format, ...);

int sscanf(const char * s, const char * format, ...);

