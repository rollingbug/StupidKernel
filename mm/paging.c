/*
 *****************************************************************************
 * paging.c (x86 Memory Paging Manager)
 * shiuan 20110111
 *****************************************************************************
 */

#include "paging.h"
#include "../kernel.h"

PAGE_DIR *p_system_page_dir;

INT8S setup_page_dir(PAGE_DIR *p_page_dir, INT32U linear_addr,
                     INT8U present, INT8U access, INT8U priv, PAGE_TABLE *p_page_table)
{
    PAGE_DESC *p_table_entry;

    if(p_page_dir == NULL)
        return -1;

    p_table_entry = &p_page_dir->table_entry[GET_PAGE_TABLE_INDEX(linear_addr)];

    p_table_entry->present = present;
    p_table_entry->read_write= access;
    p_table_entry->user_supervisor = priv;
    p_table_entry->write_through = 0;
    p_table_entry->accessed = 0;
    p_table_entry->reserved = 0;
    p_table_entry->page_size = PAGE_SIZE_4K;
    p_table_entry->global_page = 0;
    p_table_entry->available = 0;
    p_table_entry->base_addr = (((INT32U)p_page_table) >> 12);

    return 0;
}

INT8S setup_page_table(PAGE_DIR *p_page_dir, INT32U linear_addr,
                       INT8U present, INT8U access, INT8U priv, INT32U physical_addr)
{
    PAGE_TABLE *p_table;
    PAGE_DESC *p_table_item;

    if(p_page_dir == NULL)
        return -1;

    p_table = GET_PAGE_TABLE_ADDR(p_page_dir, linear_addr);
    p_table_item = GET_PAGE_ITEM_ADDR(p_table, linear_addr);

    if(p_table_item == NULL)
        return -1;

    p_table_item->present = present;
    p_table_item->read_write= access;
    p_table_item->user_supervisor = priv;
    p_table_item->write_through = 0;
    p_table_item->accessed = 0;
    p_table_item->reserved = 0;
    p_table_item->page_size = PAGE_SIZE_4K;
    p_table_item->global_page = 0;
    p_table_item->available = 0;
    p_table_item->base_addr = (((INT32U)physical_addr) >> 12);

    return 0;
}

INT8S do_mem_mapping(PAGE_DIR *p_page_dir, INT8U present, INT32U priv, INT32U linear_addr, INT32U physical_addr)
{
    PAGE_TABLE *p_table;

    if(p_page_dir == NULL)
        return -1;

    p_table = GET_PAGE_TABLE_ADDR(p_page_dir, linear_addr);

    if(p_table == NULL){
        p_table = (PAGE_TABLE *)alloc_mem_page();
        if(p_table == NULL)
            return -1;

            memset((void *)p_table, 0, sizeof(PAGE_TABLE));
            setup_page_dir(p_page_dir, linear_addr, TRUE, PAGE_READWRITE, priv, p_table);
    }

    setup_page_table(p_page_dir, linear_addr, present, PAGE_READWRITE, priv, physical_addr);

    return 0;
}

void *linear_to_physical(PAGE_DIR *p_page_dir, INT32U linear_addr)
{
    PAGE_TABLE *p_table;
    PAGE_DESC *p_table_item;

    if(p_page_dir == NULL)
        return NULL;

    p_table = GET_PAGE_TABLE_ADDR(p_page_dir, linear_addr);
    p_table_item = GET_PAGE_ITEM_ADDR(p_table, linear_addr);

    return (void *)(p_table_item->base_addr << 12);
}

void load_page_dir(PAGE_DIR *p_page_dir)
{
    asm("movl %0, %%cr3"
        :
        : "r" (p_page_dir)
    );
}

void enable_paging()
{
	asm("mov %cr0, %eax;			\
		 or $0x80000000, %eax; 		\
		 mov %eax, %cr0"
	);
}

void system_page_init()
{
    INT32U physical_mem_addr;
    INT32U free_mem_end;
    PAGE_TABLE *p_system_page_table;

    free_mem_end = get_physical_mem_size();

    p_system_page_dir = (PAGE_DIR *)alloc_mem_page();
    if(p_system_page_dir == NULL)
        kernel_panic("cannot create page dir");

    memset((void *)p_system_page_dir, 0, sizeof(PAGE_DIR));

    /* setup basic memory mapping, start from 0 to physical memory max address */
    physical_mem_addr = KERNEL_MEM_START_ADDR;
    while(physical_mem_addr < free_mem_end){
        if(do_mem_mapping(p_system_page_dir, TRUE, PAGE_SUPER_PRIV, physical_mem_addr, physical_mem_addr) == -1)
            kernel_panic("cannot create page table");

        physical_mem_addr += PAGE_MEM_SIZE;
    }

    load_page_dir(p_system_page_dir);
	enable_paging();
}
