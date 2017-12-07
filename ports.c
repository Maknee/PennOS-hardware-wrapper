#include "ports.h"

uint8_t port_read_byte(uint16_t port)
{
    uint8_t data;
    __asm__("in %%dx, %%al"
            : "=a"(data)
            : "d"(port));
    return data;
}

void port_write_byte(uint16_t port, uint8_t data)
{
    __asm__("out %%al, %%dx"
            :
            : "a"(data), "d"(port));
}

uint16_t port_read_word(uint16_t port)
{
    uint16_t data;
    __asm__("in %%dx, %%ax"
            : "=a"(data)
            : "d"(port));
    return data;
}

void port_write_word(uint16_t port, uint16_t data)
{
    __asm__("out %%ax, %%dx"
            :
            : "a"(data), "d"(port));
}
