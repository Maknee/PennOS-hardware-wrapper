#include "hal.h"

//! initialize hardware devices
int hal_init()
{

    return 0;
}

//! shutdown hardware devices
int hal_shutdown()
{
    cpu_shutdown();
    return 0;
}

void generate_interrupt(__attribute__((unused)) int n)
{
    __asm__ __volatile__("int3");
}

//! output sound to speaker
void sound(uint32_t frequency)
{
    //! sets frequency for speaker. frequency of 0 disables speaker
    port_write_byte(0x61, 3 | (uint8_t)(frequency << 2));
}

//! enable all hardware interrupts
void enable_interrupts()
{
    __asm__ __volatile__("sti");
}

//! disable all hardware interrupts
void disable_interrupts()
{
    __asm__ __volatile__("cli");
}

//! returns cpu vender
const char* get_cpu_vender()
{
    return cpu_get_vender();
}

//! returns current tick count (only for demo)
uint32_t get_tick_count()
{
    return pit_get_tick_count();
}
