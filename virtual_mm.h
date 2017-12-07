#pragma once

#include "includes.h"
#include "phys_mm.h"
#include "string.h"

/*
Bit 0 (P): Present flag
        0: Page is not in memory
        1: Page is present (in memory)
Bit 1 (R/W): Read/Write flag
        0: Page is read only
        1: Page is writable
Bit 2 (U/S):User mode/Supervisor mode flag
        0: Page is kernel (supervisor) mode
        1: Page is user mode. Cannot read or write supervisor pages
        Bits 3-4 (RSVD): Reserved by Intel
Bit 5 (A): Access flag. Set by processor
        0: Page has not been accessed
        1: Page has been accessed
Bit 6 (D): Dirty flag. Set by processor
        0: Page has not been written to
        1: Page has been written to
Bits 7-8 (RSVD): Reserved
Bits 9-11 (AVAIL): Available for use
Bits 12-31 (FRAME): Frame address
*/

//! PTE (Page Table Entry) defines
#define PTE_PRESENT 0x1
#define PTE_WRITABLE 0x2
#define PTE_USER_MODE 0x4
#define PTE_FRAME_MASK 0x7ffff000

typedef uint32_t page_table_entry;

//! enable attribute(s)
void pte_set_attribute(page_table_entry* table_entry, uint32_t attribute);

//! disable attribute(s)
void pte_unset_attribute(page_table_entry* table_entry, uint32_t attribute);

//! check if entry has attribute(s)
bool pte_test_attribute(page_table_entry* table_entry, uint32_t attribute);

//! set frame address
void pte_set_frame(page_table_entry* table_entry, uint32_t frame);

//! get physical address
uint32_t pte_get_physical_address(page_table_entry* table_entry);

/*
Bit 0 (P): Present flag
        0: Page is not in memory
        1: Page is present (in memory)
Bit 1 (R/W): Read/Write flag
        0: Page is read only
        1: Page is writable
Bit 2 (U/S):User mode/Supervisor mode flag
        0: Page is kernel (supervisor) mode
        1: Page is user mode. Cannot read or write supervisor pages
Bit 3 (PWT):Write-through flag
        0: Write back caching is enabled
        1: Write through caching is enabled
Bit 4 (PCD):Cache disabled
        0: Page table will not be cached
        1: Page table will be cached
Bit 5 (A): Access flag. Set by processor
        0: Page has not been accessed
        1: Page has been accessed
Bit 6 (D): Reserved by Intel
Bit 7 (PS): Page Size
        0: 4 KB pages
        1: 4 MB pages
Bit 8 (G): Global Page (Ignored)
Bits 9-11 (AVAIL): Available for use
Bits 12-31 (FRAME): Page Table Base address
*/

//! PDE (Page Directory Entry) defines
#define PDE_PRESENT 0x1
#define PDE_WRITABLE 0x2
#define PDE_USER_MODE 0x4
#define PDE_WRITE_THROUGH 0x8
#define PDE_ENABLE_CACHE 0x10
#define PDE_4MB_PAGE 0x40
#define PDE_GLOBAL_PAGE 0x80
#define PDE_FRAME_MASK 0x7ffff000

typedef uint32_t page_directory_entry;

//! enable attribute(s)
void pde_set_attribute(page_directory_entry* directory_entry,
    uint32_t attribute);

//! disable attribute(s)
void pde_unset_attribute(page_directory_entry* directory_entry,
    uint32_t attribute);

//! test attribute(s)
bool pde_test_attribute(page_directory_entry* directory_entry,
    uint32_t attribute);

//! set frame address
void pde_set_frame(page_directory_entry* directory_entry, uint32_t frame);

//! get physical address (page_table)
uint32_t pde_get_physical_address(page_directory_entry* directory_entry);

//! Virtual memory implementation

#define GET_PAGE_TABLE_AT_INDEX(x) (((x) >> 22) & 0x3ff)
#define GET_PAGE_ENTRY_AT_INDEX(x) (((x) >> 12) & 0x3ff)
#define GET_PHYSICAL_ADDRESS_OF_ENTRY(x) ((*(x)) & ~0xfff) // 4k aligned

#define NUM_TABLE_ENTRIES 1024
#define NUM_DIRECTORY_ENTRIES 1024
#define PAGE_SIZE 4096

//! Page Table

typedef struct
{
    page_table_entry entries[NUM_TABLE_ENTRIES];
} __attribute__((packed)) page_table;

//! Page Directory

typedef struct
{
    page_directory_entry entries[NUM_TABLE_ENTRIES];
} __attribute__((packed)) page_directory;

//! initialize virtual memory
void virtual_mm_init();

//! map virtual address to specific physical address
void virtual_mm_map_virtual_to_physical(uint32_t virtual_address,
    uint32_t physical_address);

//! allocate a new page in physical memory
void virtual_mm_allocate_page(page_table_entry* entry);

//! deallocate a new page in physical memory
void virtual_mm_free_page(page_table_entry* entry);

//! sets the page directory base register with new page directory
void virtual_mm_set_page_directory(page_directory* entry);

//! get the page directory base register's value
page_directory* virtual_mm_get_page_directory();

//! flush translation lookaside buffer
void virtual_mm_flush_tlb(uint32_t virtual_address);

//! enable paging
void virtual_mm_enable_paging(bool enable);
