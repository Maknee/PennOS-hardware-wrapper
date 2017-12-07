#include "virtual_mm.h"

static page_directory* p_directory;
static uint32_t page_directory_base_register;

//! enable attribute(s)
void pte_set_attribute(page_table_entry* table_entry, uint32_t attribute)
{
    *table_entry |= attribute;
}

//! disable attribute(s)
void pte_unset_attribute(page_table_entry* table_entry, uint32_t attribute)
{
    *table_entry &= ~attribute;
}

//! check if entry has attribute(s)
bool pte_test_attribute(page_table_entry* table_entry, uint32_t attribute)
{
    return *table_entry & attribute;
}

//! set frame address
void pte_set_frame(page_table_entry* table_entry, uint32_t frame)
{
    *table_entry = (*table_entry & ~PTE_FRAME_MASK) | frame;
}

//! get physical address
uint32_t pte_get_physical_address(page_table_entry* table_entry)
{
    return *table_entry & PTE_FRAME_MASK;
}

//! enable attribute(s)
void pde_set_attribute(page_directory_entry* directory_entry,
    uint32_t attribute)
{
    *directory_entry |= attribute;
}

//! disable attribute(s)
void pde_unset_attribute(page_directory_entry* directory_entry,
    uint32_t attribute)
{
    *directory_entry &= ~attribute;
}

//! test attribute(s)
bool pde_test_attribute(page_directory_entry* directory_entry,
    uint32_t attribute)
{
    return *directory_entry & attribute;
}

//! set frame address
void pde_set_frame(page_directory_entry* directory_entry, uint32_t frame)
{
    *directory_entry = (*directory_entry & ~PDE_FRAME_MASK) | frame;
}

//! get physical address
uint32_t pde_get_physical_address(page_directory_entry* directory_entry)
{
    return *directory_entry & PDE_FRAME_MASK;
}

//! initialize virtual memory
void virtual_mm_init()
{
    page_table* default_page_table = phys_mm_alloc();

    page_table* kernel_page_table = phys_mm_alloc();

    //! map the first 4MB to same 4MB physical memory
    page_table_entry table_entry = 0;

    for (uint32_t i = 0, virtual_address = 0x00000000, frame = 0x00000000;
         i < NUM_TABLE_ENTRIES;
         i++, virtual_address += PAGE_SIZE, frame += PAGE_SIZE)
    {
        //! Create new page entry
        table_entry = 0;

        //! Map the page entry to physical address and mark it as present
        pte_set_attribute(&table_entry, PTE_PRESENT);
        pte_set_frame(&table_entry, frame);

        //! Store the page entry into the page table
        default_page_table->entries[GET_PAGE_ENTRY_AT_INDEX(virtual_address)] = table_entry;
    }

    //! map the kernel 4MB to 3GB physical memory
    for (uint32_t i = 0, virtual_address = 0xc0000000, frame = 0x00000000;
         i < NUM_TABLE_ENTRIES;
         i++, virtual_address += PAGE_SIZE, frame += PAGE_SIZE)
    {
        //! Create new page entry
        table_entry = 0;

        //! Map the page entry to physical address and mark it as present
        pte_set_attribute(&table_entry, PTE_PRESENT);
        pte_set_frame(&table_entry, frame);

        //! Store the page entry into the page table
        default_page_table->entries[GET_PAGE_ENTRY_AT_INDEX(virtual_address)] = table_entry;
    }

    //! Allocate a directory
    page_directory* directory = phys_mm_alloc();

    //! Set the page directory base register
    page_directory_base_register = (uint32_t)directory;

    //! Zero it out!
    memset(directory, 0, sizeof(page_directory));

    //! Get the index of the table mapped to 0
    page_directory_entry* default_directory_entry = &directory->entries[GET_PAGE_TABLE_AT_INDEX(0x00000000)];

    //! Mark the table entry as present and writable and set its table entry
    pde_set_attribute(default_directory_entry, PDE_PRESENT | PDE_WRITABLE);
    pde_set_frame(default_directory_entry, (uint32_t)default_page_table);

    //! Get the index of the table mapped to virtual address 0xc0000000
    page_directory_entry* kernel_directory_entry = &directory->entries[GET_PAGE_TABLE_AT_INDEX(0xc0000000)];

    //! Mark the table entry as present and writable and set its table entry
    pde_set_attribute(kernel_directory_entry, PDE_PRESENT | PDE_WRITABLE);
    pde_set_frame(kernel_directory_entry, (uint32_t)kernel_page_table);

    //! load the page directory into the page directory base register (cr3)
    virtual_mm_set_page_directory(directory);

    //! enable paging
    virtual_mm_enable_paging(true);
}

//! map virtual address to specific physical address
void virtual_mm_map_virtual_to_physical(uint32_t virtual_address,
    uint32_t physical_address)
{
    //! Get the directory
    page_directory* directory = virtual_mm_get_page_directory();

    //! Get the table directory associated with virtual address
    page_directory_entry* directory_entry = &directory->entries[GET_PAGE_TABLE_AT_INDEX(virtual_address)];

    //! Check if the page directory entry is present
    if (!(pde_test_attribute(directory_entry, PDE_PRESENT)))
    {
        //! Allocate a new page directory entry (page table) if not present for
        //! the virtual address
        page_table* new_page_table = (page_table*)phys_mm_alloc();

        //! Clear entries in table
        memset(new_page_table, 0, sizeof(page_table));

        //! Mark the entry as present and set its frame to this new page table
        pde_set_attribute(directory_entry, PDE_PRESENT | PDE_WRITABLE);
        pde_set_frame(directory_entry, (uint32_t)new_page_table);
    }

    //! Get the table
    page_table* existing_page_table = (page_table*)pde_get_physical_address(directory_entry);

    //! Get the entry
    page_table_entry* table_entry = &existing_page_table->entries[GET_PAGE_ENTRY_AT_INDEX(virtual_address)];

    //! Mark entry as present
    pte_set_attribute(table_entry, PTE_PRESENT);

    //! Set the frame of this entry to the physical address
    pte_set_frame(table_entry, physical_address);
}

//! allocate a new page in physical memory
void virtual_mm_allocate_page(page_table_entry* entry)
{
    //! allocate a block of memory
    uint32_t physical_address = (uint32_t)phys_mm_alloc();

    //! set the entry as present
    pte_set_attribute(entry, PTE_PRESENT);

    //! set the physical address that the entry is mapped to
    pte_set_frame(entry, physical_address);
}

//! deallocate a new page in physical memory
void virtual_mm_free_page(page_table_entry* entry)
{
    //! get the physical address that the entry is mapped to
    uint32_t physical_address = pte_get_physical_address(entry);

    //! free the physical block
    phys_mm_free((void*)physical_address);

    //! clear the entry
    memset(entry, 0, sizeof(page_table_entry));
}

//! sets the page directory base register with new page directory
void virtual_mm_set_page_directory(page_directory* entry)
{
    p_directory = entry;

    __asm__ __volatile__("movl (%0), %%eax\n\t"
                         "movl %%eax, %%cr3\n\t"
                         :
                         : "m"(page_directory_base_register)
                         : "%eax");
}

//! get the page directory base register's value
page_directory* virtual_mm_get_page_directory()
{
    return p_directory;
}

//! flush translation lookaside buffer
void virtual_mm_flush_tlb(uint32_t virtual_address)
{
    __asm__ __volatile__("cli\n\t"
                         "invlpg %0\n\t"
                         "sti\n\t"
                         :
                         : "m"(virtual_address)
                         :);
}

//! enable paging
void virtual_mm_enable_paging(bool enable)
{
    if (enable)
    {
        __asm__ __volatile__("movl %%cr0, %%eax\n\t"
                             "orl $0x80000000, %%eax\n\t"
                             "mov %%eax, %%cr0\n\t"
                             :
                             :
                             : "%eax");
    }
    else
    {
        __asm__ __volatile__("movl %%cr0, %%eax\n\t"
                             "andl $0x7fffffff, %%eax\n\t"
                             "mov %%eax, %%cr0\n\t"
                             :
                             :
                             : "%eax");
    }
}
