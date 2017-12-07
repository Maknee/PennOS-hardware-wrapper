#pragma once

#include "includes.h"

#define MAX_DESCRIPTORS 3 // null, code, data
#define GDT_DESC_ACCESS 0x1 // access bits
#define GDT_DESC_READWRITE 0x2 // readwrite bits
#define GDT_DESC_EXPANSION 0x4 // expansion bits
#define GDT_DESC_CODEEXEC 0x8 // code/exec bits
#define GDT_DESC_CODEDATA 0x10 // code/data bits
#define GDT_DESC_DPL 0x60 // dpl bits
#define GDT_DESC_MEMORY 0x80 // in memory bits

#define GDT_DESC_MASK 0x0f // mask high limit bits
#define GDT_DESC_OS 0x10 // os bits
#define GDT_DESC_32BIT 0x40 // 32 bits
#define GDT_DESC_4K 0x80 // 4k bits

#define GDT_CODE_SEGMENT 0x8
#define GDT_DATA_SEGMENT 0x10

typedef struct
{
    uint16_t limit; // 0-15bits segment limit
    uint16_t base_low; // 0-15bits of base
    uint8_t base_mid; // 16-23bits of base
    uint8_t flags; // flags
    uint8_t grand;
    uint8_t base_high; // 24-31 of base
} __attribute__((packed)) gdt_entry;

typedef struct
{
    uint16_t limit; // size
    uint32_t base; // base address
} __attribute__((packed)) gdt_register;

void load_gdt(); // loads the gdt into gdtr
gdt_entry* get_gdt_entry(uint32_t index); // get pointer to entry
void install_gdt(uint32_t index, uint32_t base, uint32_t limit, uint8_t access,
    uint8_t grand);
void gdt_init();
