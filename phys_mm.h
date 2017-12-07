#pragma once

#include "debug.h"
#include "elf.h"
#include "includes.h"
#include "multiboot.h"
#include "string.h"

#define MAX_MEMORY_ADDRESS 0xFFFF0000
#define PAGE_SIZE 4096
#define KILOBYTE_SIZE 1024

//https://stackoverflow.com/questions/1010922/question-about-round-up-macro

#define ROUND_DOWN(val, round) (((val) / (round)) * (round))
#define ROUND_UP(val, round) ((((val) + (round)-1) / (round)) * (round))

//round to nearest page
#define ROUND_PAGE_DOWN(val) ((val) & (~(PAGE_SIZE - 1)))
#define ROUND_PAGE_UP(val) (((val) + (PAGE_SIZE)-1) & (~(PAGE_SIZE - 1)))

typedef enum {
    MEMORY_FREE,
    MEMORY_RESERVED,
    MEMORY_INVALID
} MEMORY_STATE;

typedef struct
{
    multiboot_info_t* boot_info;
    multiboot_memory_map_t* multiboot_memory_map;
    multiboot_elf_section_header_table_t* elf_section_table;
    uint32_t memory_map_size;
    uint32_t multiboot_info_start;
    uint32_t multiboot_info_end;
    uint32_t kernel_start;
    uint32_t kernel_end;
    uint32_t memory_lower; //measured in KB
    uint32_t memory_upper;
    uint32_t heap_start;
    uint32_t highest_free_address;
} memory_info;

//! set block in bitmap
void phys_mm_set_block_bit(uint32_t bit);

//! unset block in bitmap
void phys_mm_unset_block_bit(uint32_t bit);

//! test block in bitmap
bool phys_mm_test_block_bit(uint32_t bit);

//! find first free block in bitmap
uint32_t phys_mm_find_first_free();

//! find first free blocks of particular size in bitmap
uint32_t phys_mm_find_first_free_blocks(uint32_t size);

//! setup the entire physical memory manager
void phys_mm_init(multiboot_info_t* boot_info);

//! marks a region as used
void phys_mm_set_blocks(uint32_t base, uint32_t size);

//! marks a region as free
void phys_mm_unset_blocks(uint32_t base, uint32_t size);

//! returns an allocated region to be used
void* phys_mm_alloc();

//! free an allocated region
void phys_mm_free(void* region);

//! returns allocated regions to be used
void* phys_mm_allocs(uint32_t size);

//! free allocated regions
void phys_mm_frees(void* region, uint32_t size);

//! get memory size
uint32_t phys_mm_get_memory_size();

//! get total number of blocks
uint32_t phys_mm_get_total_blocks();

//! get total number of used blocks
uint32_t phys_mm_get_used_blocks();

//! get total number of free blocks
uint32_t phys_mm_get_free_blocks();

//! disable/enable paging
void phys_mm_enable_paging(bool b);

//! test if paging is enabled
bool phys_mm_test_paging();

//! disable/enable pdbr
void phys_mm_enable_pdbr(bool b);

//! test if pdbr is enabled
bool phys_mm_test_pdbr();

//! debug physical memory
void phys_mm_debug();
