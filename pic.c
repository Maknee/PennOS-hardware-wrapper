#include "pic.h"

//! Read data byte from pic
uint8_t pic_read_data(uint8_t pic)
{
    uint8_t port;
    if (pic == MASTER_PIC)
    {
        port = PIC1_REG_DATA;
    }
    else
    {
        port = PIC2_REG_DATA;
    }
    return port_read_byte(port);
}

//! Send a data byte to pic
void pic_send_data(uint8_t pic, uint8_t data)
{
    uint8_t port;
    if (pic == MASTER_PIC)
    {
        port = PIC1_REG_DATA;
    }
    else
    {
        port = PIC2_REG_DATA;
    }
    port_write_byte(port, data);
}
//! Send operational command to pic
void pic_send_command(uint8_t pic, uint8_t command)
{
    uint8_t port;
    if (pic == MASTER_PIC)
    {
        port = PIC1_REG_COMMAND;
    }
    else
    {
        port = PIC2_REG_COMMAND;
    }
    port_write_byte(port, command);
}

//! Enables and disables interrupts
void pic_mask_irq(uint8_t pic, uint8_t irq)
{
    uint8_t port;
    if (pic == MASTER_PIC)
    {
        port = PIC1_REG_DATA;
    }
    else
    {
        port = PIC2_REG_DATA;
    }
    uint8_t data = port_read_byte(port) | (1 << irq);
    port_write_byte(port, data);
}

void pic_unmask_irq(uint8_t pic, uint8_t irq)
{
    uint8_t port;
    if (pic == MASTER_PIC)
    {
        port = PIC1_REG_DATA;
    }
    else
    {
        port = PIC2_REG_DATA;
    }
    uint8_t data = port_read_byte(port) & ~(1 << irq);
    port_write_byte(port, data);
}

//! Initialize pic
void pic_remap(uint8_t master_isr, uint8_t slave_isr)
{
    //! Begin initialization of PIC

    pic_send_command(MASTER_PIC, PIC_ICW1_INIT | PIC_ICW1_IC4);
    pic_send_command(SLAVE_PIC, PIC_ICW1_INIT | PIC_ICW1_IC4);

    //! Send initialization control word 2. This is the base addresses of the
    //! irq's

    pic_send_data(MASTER_PIC, master_isr);
    pic_send_data(SLAVE_PIC, slave_isr);

    //! Send initialization control word 3. This is the connection between
    //! master and slave.
    //! ICW3 for master PIC is the IR that connects to secondary pic in binary
    //! format
    //! ICW3 for secondary PIC is the IR that connects to master pic in decimal
    //! format

    //! what the fuck, the master IRQ 2 is in binary representation aka (100) has a value of 2 while decimal is used for slave (10)
    pic_send_data(MASTER_PIC, 0x4);
    pic_send_data(SLAVE_PIC, 0x2);

    //! Send Initialization control word 4. Enables i86 mode
    pic_send_data(MASTER_PIC, PIC_ICW4_UPM);
    pic_send_data(SLAVE_PIC, PIC_ICW4_UPM);

    //! Null out data registers
    pic_send_data(MASTER_PIC, 0);
    pic_send_data(SLAVE_PIC, 0);
}

void pic_init()
{
    pic_remap(PIC_MASTER_ISR, PIC_SLAVE_ISR);
}
