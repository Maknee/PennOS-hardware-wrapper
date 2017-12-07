#pragma once

#include "includes.h"
#include "keyboard.h"

//https://github.com/shawnanastasio/ShawnOS

// US QWERTY
uint8_t _keyboard_us_qwerty_sc_arr[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8',
    '9', '0', '-', '=', '\b',
    '\t',
    'q', 'w', 'e', 'r',
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', 0, // Left Shift
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',
    'm', ',', '.', '/', 0, // Right Shift
    '*',
    0, // Alt
    ' ', // Space bar
    0, // Caps lock
    0, // 59 - F1 key ... >
    0, 0, 0, 0, 0, 0, 0, 0,
    0, // < ... F10
    0, // 69 - Num lock
    0, // Scroll Lock
    0, // Home key
    0, // Up Arrow
    0, // Page Up
    '-',
    0, // Left Arrow
    0,
    0, // Right Arrow
    '+',
    0, // 79 - End key
    0, // Down Arrow
    0, // Page Down
    0, // Insert Key
    0, // Delete Key
    0, 0, 0,
    0, // F11 Key
    0, // F12 Key
    0, // All other keys are undefined
};

keyboard_scancode_set keyboard_us_qwerty_sc = {
    .scancodes     = _keyboard_us_qwerty_sc_arr,
    .length = 128,
};

// Scancodes modified by shift key
uint8_t _keyboard_us_qwerty_sc_shift_arr[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*',
    '(', ')', '_', '+', '\b',
    '\t',
    'Q', 'W', 'E', 'R',
    'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
    '"', '~', 0, // Left Shift"
    '|', 'Z', 'X', 'C', 'V', 'B', 'N',
    'M', '<', '>', '?', 0, // Right Shift
    '*',
    0, // Alt
    ' ', // Space bar
    0, // Caps lock
    0, // 59 - F1 key ... >
    0, 0, 0, 0, 0, 0, 0, 0,
    0, // < ... F10
    0, // 69 - Num lock
    0, // Scroll Lock
    0, // Home key
    0, // Up Arrow
    0, // Page Up
    '-',
    0, // Left Arrow
    0,
    0, // Right Arrow
    '+',
    0, // 79 - End key
    0, // Down Arrow
    0, // Page Down
    0, // Insert Key
    0, // Delete Key
    0, 0, 0,
    0, // F11 Key
    0, // F12 Key
    0, // All other keys are undefined
};
keyboard_scancode_set keyboard_us_qwerty_sc_shift = {
    .scancodes     = _keyboard_us_qwerty_sc_shift_arr,
    .length = 128,
};

// Scancodes modified by capslock
uint8_t _keyboard_us_qwerty_sc_capslock_arr[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8',
    '9', '0', '-', '=', '\b',
    '\t',
    'Q', 'W', 'E', 'R',
    'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n',
    0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';',
    '\'', '`', 0, // Left Shift
    '\\', 'Z', 'X', 'C', 'V', 'B', 'N',
    'M', ',', '.', '/', 0, // Right Shift
    '*',
    0, // Alt
    ' ', // Space bar
    0, // Caps lock
    0, // 59 - F1 key ... >
    0, 0, 0, 0, 0, 0, 0, 0,
    0, // < ... F10
    0, // 69 - Num lock
    0, // Scroll Lock
    0, // Home key
    0, // Up Arrow
    0, // Page Up
    '-',
    0, // Left Arrow
    0,
    0, // Right Arrow
    '+',
    0, // 79 - End key
    0, // Down Arrow
    0, // Page Down
    0, // Insert Key
    0, // Delete Key
    0, 0, 0,
    0, // F11 Key
    0, // F12 Key
    0, // All other keys are undefined
};
keyboard_scancode_set keyboard_us_qwerty_sc_capslock = {
    .scancodes     = _keyboard_us_qwerty_sc_capslock_arr,
    .length = 128,
};

// Scancodes modified by shift and caps lock (we need a better way to handle this and other cases with multiple modifier keys)
uint8_t _keyboard_us_qwerty_sc_shift_capslock_arr[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*',
    '(', ')', '_', '+', '\b',
    '\t',
    'q', 'w', 'e', 'r',
    't', 'y', 'u', 'i', 'o', 'p', '{', '}', '\n',
    0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':',
    '"', '~', 0, // Left Shift"
    '|', 'z', 'x', 'c', 'v', 'b', 'n',
    'm', '<', '>', '?', 0, // Right Shift
    '*',
    0, // Alt
    ' ', // Space bar
    0, // Caps lock
    0, // 59 - F1 key ... >
    0, 0, 0, 0, 0, 0, 0, 0,
    0, // < ... F10
    0, // 69 - Num lock
    0, // Scroll Lock
    0, // Home key
    0, // Up Arrow
    0, // Page Up
    '-',
    0, // Left Arrow
    0,
    0, // Right Arrow
    '+',
    0, // 79 - End key
    0, // Down Arrow
    0, // Page Down
    0, // Insert Key
    0, // Delete Key
    0, 0, 0,
    0, // F11 Key
    0, // F12 Key
    0, // All other keys are undefined
};

keyboard_scancode_set keyboard_us_qwerty_sc_shift_capslock = {
    .scancodes     = _keyboard_us_qwerty_sc_shift_capslock_arr,
    .length = 128,
};

// Shift keys
uint8_t _keyboard_us_qwerty_shift_arr[2] = {
    42, //LShift
    54 //RShift
};

keyboard_scancode_set keyboard_us_qwerty_shift = {
    .scancodes     = _keyboard_us_qwerty_shift_arr,
    .length = 2,
};

// US QWERTY

keyboard_driver keyboard_us_qwerty = {
    .keyboard_sc                = &keyboard_us_qwerty_sc,
    .keyboard_sc_shift          = &keyboard_us_qwerty_sc_shift,
    .keyboard_shift             = &keyboard_us_qwerty_shift,
    .keyboard_sc_capslock       = &keyboard_us_qwerty_sc_capslock,
    .keyboard_sc_shift_capslock = &keyboard_us_qwerty_sc_shift_capslock,
    .keyboard_caps_sc           = 58,
    .keyboard_l_ctrl_sc           = 0x1D,
    .keyboard_special           = NULL // Temporary
};
