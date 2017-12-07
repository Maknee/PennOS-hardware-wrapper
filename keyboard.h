#pragma once

#include "includes.h"
#include "ctype.h"
#include "isr.h"
#include "ports.h"
#include "vga.h"

// keyboard encoder
#define KEYBOARD_ENCODER_INPUT_BUFFER 0x60

#define KEYBOARD_ENCODER_COMMAND_SET_LED 0xEE

#define KEYBOARD_ENCODER_COMMAND_REGISTER 0x60

// keyboard controller
#define KEYBOARD_CONTROLLER_STATUS_REGISTER 0x64

#define KEYBOARD_CONTROLLER_STATUS_OUT_BUF 0x1
#define KEYBOARD_CONTROLLER_STATUS_IN_BUF 0x2

#define KEYBOARD_CONTROLLER_COMMAND_REGISTER 0x64

#define KEYBOARD_CONTROLLER_COMMAND_READ 0x20
#define KEYBOARD_CONTROLLER_COMMAND_WRITE 0x60

// errors
#define KEYBOARD_BAT_FAILED 0xFC
#define KEYBOARD_DIAG_FAILED 0xFD
#define KEYBOARD_RESEND_COMMAND 0xFE

//https://github.com/shawnanastasio/ShawnOS

typedef struct keyboard_scancode_set
{
    uint8_t* scancodes;
    size_t length;
} keyboard_scancode_set;

typedef struct keyboard_driver
{
    keyboard_scancode_set* keyboard_sc; // Default scancode characters
    keyboard_scancode_set* keyboard_sc_shift; // Scancode characters modified by shift
    keyboard_scancode_set* keyboard_shift; // Scancodes of shift keys
    keyboard_scancode_set* keyboard_sc_capslock; // Scancode characters modified by capslock
    keyboard_scancode_set* keyboard_sc_shift_capslock; // Scancode characters modified by shift & capslock
    keyboard_scancode_set* keyboard_special;
    uint32_t keyboard_caps_sc; // Caps lock scancode
    uint32_t keyboard_l_ctrl_sc; // Left control set scancode
} keyboard_driver;

bool keyboard_read_buffer(char* buffer, uint32_t size);

uint8_t keyboard_encoder_read_input();
void keyboard_encoder_send_command(uint8_t cmd);
uint8_t keyboard_controller_read_status();
void keyboard_controller_send_command(uint8_t cmd);

void keyboard_init();
