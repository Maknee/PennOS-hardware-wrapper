#pragma once

#include "includes.h"

uint8_t port_read_byte(uint16_t port);
void port_write_byte(uint16_t port, uint8_t data);
uint16_t port_read_word(uint16_t port);
void port_write_word(uint16_t port, uint16_t data);
