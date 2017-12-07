#include "phys_mm.h"

memory_info mem_info;

//! total number of blocks
static uint32_t total_blocks;

//! total number of used blocks
static uint32_t used_blocks;

//! bitmap pointer
static uint32_t* memory_map;

//! set block in bitmap
void phys_mm_set_block_bit(uint32_t bit)
{
    memory_map[bit / 32] |= (1 << (bit % 32));
}

//! unset block in bitmap
void phys_mm_unset_block_bit(uint32_t bit)
{
    memory_map[bit / 32] &= ~(1 << (bit % 32));
}

//! test block in bitmap
bool phys_mm_test_block_bit(uint32_t bit)
{
    return memory_map[bit / 32] & (1 << (bit % 32));
}

//! find first free block in bitmap
uint32_t phys_mm_find_first_free()
{
    // each block is 32 bits, so divide by 32 and iterate through 32 later
    for (uint32_t i = 0; i < phys_mm_get_total_blocks() / 32; i++)
    {
        while (memory_map[i] != 0xFFFFFFFF)
        {
            // iterate through 32 bits (32 blocks)
            for (uint32_t j = 0; j < 32; j++)
            {
                uint32_t bit = 1 << j;

                //! test if bit is cleared
                if (!(memory_map[i] & bit))
                {
                    // return bit index
                    return i * 32 + j;
                }
            }
        }
    }
    return 0;
}

//! find first free blocks of particular size in bitmap
uint32_t phys_mm_find_first_free_blocks(uint32_t size)
{
    if (size == 0)
    {
        return 0;
    }

    if (size == 1)
    {
        return phys_mm_find_first_free();
    }

    // each block is 32 bits, so divide by 32 and iterate through 32 later
    for (uint32_t i = 0; i < phys_mm_get_total_blocks() / 32; i++)
    {
        while (memory_map[i] != 0xFFFFFFFF)
        {
            // iterate through 32 bits (32 blocks)
            for (uint32_t j = 0; j < 32; j++)
            {
                uint32_t bit = 1 << j;

                //! test if bit is cleared
                if (!(memory_map[i] & bit))
                {
                    // starting bit index
                    uint32_t starting_bit_index = i * 32 + j;
                    uint32_t num_free_indices   = 0;
                    for (uint32_t count = 0; count < size; count++)
                    {
                        if (!phys_mm_test_block_bit(starting_bit_index + count))
                        {
                            num_free_indices++;
                        }

                        if (num_free_indices == size)
                        {
                            return i * 32 + j;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

//find where the kernel ends and starts
void read_elf_sections()
{
    elf_section_header_t* elf_header = (elf_section_header_t*)mem_info.elf_section_table->addr;

    //go to next header
    elf_header++;

    //start of kernel
    mem_info.kernel_start = elf_header->sh_addr;
    mem_info.kernel_end   = elf_header->sh_addr;

    //update the end of kernel
    for (uint32_t i = 0; i < mem_info.elf_section_table->num - 1; i++)
    {
        if (elf_header->sh_addr > mem_info.kernel_end)
        {
            mem_info.kernel_end = elf_header->sh_addr;
        }
        elf_header++;
    }

    //round to nearest page
    mem_info.kernel_end = ROUND_PAGE_UP(mem_info.kernel_end);
}

//check if memory address is in multiboot memory map
MEMORY_STATE check_multiboot_info_memory_state(uint32_t address)
{
    //get the memory maps
    uintptr_t current_multiboot_memory_address = (uintptr_t)mem_info.multiboot_memory_map;
    uintptr_t end_multiboot_memory_address     = current_multiboot_memory_address + mem_info.memory_map_size;

    while (current_multiboot_memory_address < end_multiboot_memory_address)
    {
        multiboot_memory_map_t* current_multiboot_memory_map = (multiboot_memory_map_t*)current_multiboot_memory_address;

        //memory address region's start and end (where the memory the entry is defined)
        uint32_t start_address = current_multiboot_memory_map->addr;
        uint32_t end_address   = start_address + current_multiboot_memory_map->len;
        //check if memory is in range
        if (address >= start_address && address <= end_address)
        {
            //check if is reserved memory
            if (current_multiboot_memory_map->type != MULTIBOOT_MEMORY_AVAILABLE)
            {
                return MEMORY_RESERVED;
            }

            //check if address is within memory bounds
            if (address + PAGE_SIZE > end_address)
            {
                return MEMORY_RESERVED;
            }

            return MEMORY_FREE;
        }

        current_multiboot_memory_address += current_multiboot_memory_map->size + sizeof(uintptr_t);
    }

    //memory not found in multiboot memory map
    return MEMORY_INVALID;
}

//check if memory address is valid
MEMORY_STATE check_memory_state(uint32_t address)
{
    //get to next page (current address + page size)
    uint32_t address_end = address + PAGE_SIZE;

    if (address <= 0x1000)
    {
        return MEMORY_RESERVED;
    }

    //check if address touches multiboot
    if ((address >= mem_info.multiboot_info_start && address <= mem_info.multiboot_info_end) || (address_end >= mem_info.multiboot_info_start && address_end <= mem_info.multiboot_info_end))
    {
        return MEMORY_RESERVED;
    }

    //check if address touches kernel memory
    if ((address >= mem_info.kernel_start && address <= mem_info.kernel_end) || (address_end >= mem_info.kernel_start && address_end <= mem_info.kernel_end))
    {
        return MEMORY_RESERVED;
    }

    //check if address touches multiboot info memory map
    MEMORY_STATE multiboot_reserved = check_multiboot_info_memory_state(address);
    if (multiboot_reserved != MEMORY_FREE)
    {
        return multiboot_reserved;
    }

    return MEMORY_FREE;
}

void mark_reserved_sections()
{
    for (uint32_t address = 0; address < MAX_MEMORY_ADDRESS; address += PAGE_SIZE)
    {
        MEMORY_STATE state = check_memory_state(address);
        if (state == MEMORY_RESERVED)
        {
            phys_mm_set_blocks(address, PAGE_SIZE);
        }
    }
}

//! setup the entire physical memory manager
void phys_mm_init(multiboot_info_t* boot_info)
{
    //verify if multiboo has correct info
    if (!(boot_info->flags & (1 << 6))) //presence of memory map (bit 6)
    {
        ABORT_MSG("Multiboot memory map could not be found");
    }

    if (!(boot_info->flags & (1 << 5))) //presence of elf (bit 5)
    {
        ABORT_MSG("ELF info could not be found");
    }

    mem_info.boot_info            = boot_info;
    mem_info.multiboot_memory_map = (multiboot_memory_map_t*)boot_info->mmap_addr;
    mem_info.elf_section_table    = &(boot_info->u.elf_sec);
    mem_info.memory_map_size      = boot_info->mmap_length;
    mem_info.multiboot_info_start = (uint32_t)boot_info;
    mem_info.multiboot_info_end   = (uint32_t)((uintptr_t)boot_info + sizeof(multiboot_info_t));
    //if bit 0 in the ‘flags’ word is set, then the ‘mem_*’ fields are valid. ‘mem_lower’ and ‘mem_upper’ indicate the amount of lower and upper memory, respectively, in kilobytes. Lower memory starts at address 0, and upper memory starts at address 1 megabyte. The maximum possible value for lower memory is 640 kilobytes. The value returned for upper memory is maximally the address of the first upper memory hole minus 1 megabyte. It is not guaranteed to be this value.

    mem_info.memory_lower = boot_info->mem_lower;
    mem_info.memory_upper = boot_info->mem_upper;

    // set highest address
    mem_info.highest_free_address = mem_info.memory_upper * 1024;

    // read elf and update kernel start / end
    read_elf_sections();

    // start the heap after the kernel end
    mem_info.heap_start = ROUND_PAGE_UP(mem_info.kernel_end);

    // start at heap start
    total_blocks = MAX_MEMORY_ADDRESS / PAGE_SIZE;
    used_blocks  = 0;
    memory_map   = (uint32_t*)mem_info.heap_start;

    //free everything first
    uint32_t bitset_size = (total_blocks + sizeof(uint32_t)) / sizeof(uint32_t);
    memset(memory_map, 0x0, bitset_size);

    // mark the memory map as used (waste, but covers entire memory range
    phys_mm_set_blocks(memory_map, bitset_size);

    mem_info.heap_start += bitset_size;

    //mark reserved sections into the bitmap
    mark_reserved_sections();

    // init kernel region
    phys_mm_set_blocks(mem_info.kernel_start, mem_info.kernel_end - mem_info.kernel_start);
}

//! marks a region as used
void phys_mm_set_blocks(uint32_t base, uint32_t size)
{
    // transform base/size into bitmap's equalivant values
    uint32_t base_bit = base / PAGE_SIZE;
    uint32_t size_bit = size / PAGE_SIZE;

    // mark blocks as used
    for (uint32_t i = 0; i < size_bit; i++)
    {
        phys_mm_set_block_bit(base_bit++);
        used_blocks++;
    }
}

//! marks a region as free
void phys_mm_unset_blocks(uint32_t base, uint32_t size)
{
    // transform base/size into bitmap's equalivant values
    uint32_t base_bit = base / PAGE_SIZE;
    uint32_t size_bit = size / PAGE_SIZE;

    // mark blocks as free
    for (uint32_t i = 0; i < size_bit; i++)
    {
        phys_mm_unset_block_bit(base_bit++);
        used_blocks--;
    }
}

//! returns an allocated region to be used
void* phys_mm_alloc()
{
    // check if there any free blocks
    if (phys_mm_get_free_blocks() <= 0)
    {
        return 0;
    }

    // find first free block
    uint32_t block = phys_mm_find_first_free();

    // check if we don't have any more free blocks
    if (block == 0)
    {
        return 0;
    }

    // set the block to be "used" and update appropriate variables
    phys_mm_set_block_bit(block);
    used_blocks++;

    // convert to actual address
    uint32_t address = block * PAGE_SIZE;

    return (void*)address;
}

//! free an allocated region
void phys_mm_free(void* region)
{
    // convert actual address to bitmap equalivant
    uint32_t block = ((uint32_t)region) / PAGE_SIZE;

    // mark the region as free
    phys_mm_unset_block_bit(block);
    used_blocks--;
}

//! returns allocated regions to be used
void* phys_mm_allocs(uint32_t size)
{
    // check if there any free blocks
    if (phys_mm_get_free_blocks() <= 0)
    {
        return 0;
    }

    // find first free block(s)
    uint32_t block = phys_mm_find_first_free_blocks(size);

    // check if we don't have any more free blocks
    if (block == 0)
    {
        return 0;
    }

    // set the block to be "used" and update appropriate variables
    for (uint32_t i = 0; i < size; i++)
    {
        phys_mm_set_block_bit(block + i);
        used_blocks++;
    }

    // convert to actual address
    uint32_t address = block * PAGE_SIZE;

    return (void*)address;
}

//! free allocated regions
void phys_mm_frees(void* region, uint32_t size)
{
    // convert actual address to bitmap equalivant
    uint32_t block = ((uint32_t)region) / PAGE_SIZE;

    // mark the region as free
    for (uint32_t i = 0; i < size; i++)
    {
        phys_mm_unset_block_bit(block + i);
        used_blocks--;
    }
}

//! get total number of blocks
uint32_t phys_mm_get_total_blocks()
{
    return total_blocks;
}

//! get total number of used blocks
uint32_t phys_mm_get_used_blocks()
{
    return used_blocks;
}

//! get total number of free blocks
uint32_t phys_mm_get_free_blocks()
{
    return total_blocks - used_blocks;
}

//! disable/enable paging
void phys_mm_enable_paging(bool b);

//! test if paging is enabled
bool phys_mm_test_paging();

//! disable/enable pdbr
void phys_mm_enable_pdbr(bool b);

//! test if pdbr is enabled
bool phys_mm_test_pdbr();

static const char* MEMORY_STATE_STRING[] = {
    "",
    "MEMORY_AVALIABLE",
    "MEMORY_RESERVED",
    "MEMORY_ACPI_RECLAIMABLE",
    "MEMORY_NVS",
    "MEMORY_BADRAM"
};

//! debug physical memory
void phys_mm_debug()
{
    vga_printf(
        "pmm initialized with memLo: %x memHi: %x\n",
        mem_info.memory_lower * KILOBYTE_SIZE, mem_info.memory_upper * KILOBYTE_SIZE);

    //get the memory maps
    uintptr_t current_multiboot_memory_address = (uintptr_t)mem_info.multiboot_memory_map;
    uintptr_t end_multiboot_memory_address     = current_multiboot_memory_address + mem_info.memory_map_size;

    while (current_multiboot_memory_address < end_multiboot_memory_address)
    {
        multiboot_memory_map_t* current_multiboot_memory_map = (multiboot_memory_map_t*)current_multiboot_memory_address;

        //memory address region's start and end (where the memory the entry is defined)
        uint32_t start_address = current_multiboot_memory_map->addr;
        uint32_t end_address   = start_address + current_multiboot_memory_map->len;
        vga_printf("Start: %x End: %x Type: %s\n", start_address, end_address, MEMORY_STATE_STRING[current_multiboot_memory_map->type]);

        current_multiboot_memory_address += current_multiboot_memory_map->size + sizeof(uintptr_t);
    }

    vga_printf("\npmm regions initialized: %i allocation blocks; used or "
                 "reserved blocks: %i\nfree blocks: %i\n",
        phys_mm_get_total_blocks(), phys_mm_get_used_blocks(),
        phys_mm_get_free_blocks());
}
