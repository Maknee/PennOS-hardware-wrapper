#pragma once

#include "includes.h"
#include "ports.h" //writing to port for cursor
#include "stdarg.h" //va_list, va_start, va_arg, va_end
#include "stdlib.h" //itoa
#include "string.h" //strlen

void vga_clear(const uint8_t character);
void vga_putc(const char character);
void vga_backspace();
void vga_puts(char* str);
int vga_printf(const char* str, ...);
void vga_set_color(const uint8_t color);
void vga_go_to_pixel(const uint8_t x, const uint8_t y);
uint8_t vga_get_x();
uint8_t vga_get_y();
void vga_update_cursor(const uint8_t x, const uint8_t y);

// start of video memory
#define VIDEO_MEMORY 0xB8000

// number of columns and rows in vga
#define COLUMNS 80
#define ROWS 25

