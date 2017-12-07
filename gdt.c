#include "gdt.h"

static gdt_entry gdt[MAX_DESCRIPTORS]; // 3 descriptors

static gdt_register gdtr; // one gdt register

extern void set_selectors_to_gdt();

void load_gdt() // loads the gdt into gdtr
{
    __asm__ __volatile__("lgdt (%0)"
                         :
                         : "r"(&gdtr));
    set_selectors_to_gdt();
}

gdt_entry* get_gdt_entry(uint32_t index) // get pointer to descriptor
{
    if (index > MAX_DESCRIPTORS)
    {
        return 0;
    }

    return &gdt[index];
}

void install_gdt(uint32_t index, uint32_t base, uint32_t limit, uint8_t access,
    uint8_t grand)
{
    if (index > MAX_DESCRIPTORS)
    {
        return;
    }

    gdt[index].base_low  = (uint16_t)(base & 0xFFFF);
    gdt[index].base_mid  = (uint8_t)((base >> 16) & 0xFF);
    gdt[index].base_high = (uint8_t)((base >> 24) & 0xFF);
    gdt[index].limit     = (uint16_t)(limit & 0xFFFF);
    gdt[index].flags     = access;
    gdt[index].grand     = (uint8_t)((limit >> 16) & 0x0F);
    gdt[index].grand |= grand & 0xf0;
}

void gdt_init()
{
    // setup gdtr
    gdtr.limit = (sizeof(gdt_entry) * MAX_DESCRIPTORS) - 1;
    gdtr.base  = (uint32_t)&gdt[0];

    // zero out gdt
    for (uint32_t i = 0; i < MAX_DESCRIPTORS; i++)
    {
        // using compound literals to zero intialize
        gdt[i] = (gdt_entry){ 0 };
    }

    // null descriptor
    install_gdt(0, 0, 0, 0, 0);

    // code descriptor
    install_gdt(1, 0, 0xFFFFFFFF, GDT_DESC_READWRITE | GDT_DESC_CODEEXEC | GDT_DESC_CODEDATA | GDT_DESC_MEMORY,
        GDT_DESC_4K | GDT_DESC_32BIT | GDT_DESC_MASK);

    // data descriptor
    install_gdt(2, 0, 0xFFFFFFFF,
        GDT_DESC_READWRITE | GDT_DESC_CODEDATA | GDT_DESC_MEMORY,
        GDT_DESC_4K | GDT_DESC_32BIT | GDT_DESC_MASK);

    // load gdt
    load_gdt();
}
