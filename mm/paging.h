/*
 *****************************************************************************
 * paging.h (x86 Memory Paging Manager)
 * shiuan 20110111
 *****************************************************************************
 */

#ifndef _PAGING_H_
#define _PAGING_H_

#include "../types.h"

#define PAGE_PRESENT        1
#define PAGE_NOT_PRESENT    0
#define PAGE_READWRITE      1
#define PAGE_READONLY       0
#define PAGE_SIZE_4K        0
#define PAGE_SZIE_4M        1
#define PAGE_SUPER_PRIV     1
#define PAGE_USER_PRIV      0

#define PAGE_BASIC_NUM  1024
#define PAGE_MEM_SIZE   4096    /* each page is 4K */

#define GET_PAGE_TABLE_INDEX(linear_addr)       ((linear_addr & 0xFFC00000) >> 22)
#define GET_PAGE_ITEM_INDEX(linear_addr)        ((linear_addr & 0x003FF000) >> 12)

#define GET_PAGE_TABLE_ADDR(p_page_dir, linear_addr)                                                        \
        (PAGE_TABLE *)((INT32U)p_page_dir->table_entry[GET_PAGE_TABLE_INDEX(linear_addr)].base_addr << 12)

#define GET_PAGE_ITEM_ADDR(p_table, linear_addr)                                                            \
        &p_table->table_item[GET_PAGE_ITEM_INDEX(linear_addr)]

typedef struct page_desc{
	INT32U present:1;
	INT32U read_write:1;
	INT32U user_supervisor:1;
	INT32U write_through:1;
	INT32U cache_disabled:1;
	INT32U accessed:1;
	INT32U reserved:1;
	INT32U page_size:1;
	INT32U global_page:1;
	INT32U available:3;
	INT32U base_addr:20;
}__attribute__((packed)) PAGE_DESC;

typedef struct page_dir{
	struct page_desc table_entry[PAGE_BASIC_NUM];
}__attribute__((packed)) PAGE_DIR;

typedef struct page_table{
	struct page_desc table_item[PAGE_BASIC_NUM];
}__attribute__((packed)) PAGE_TABLE;

INT8S setup_page_dir(PAGE_DIR *p_page_dir, INT32U linear_addr,
                     INT8U present, INT8U access, INT8U priv, PAGE_TABLE *p_page_table);
INT8S setup_page_table(PAGE_DIR *p_page_dir, INT32U linear_addr,
                       INT8U present, INT8U access, INT8U priv, INT32U physical_addr);
INT8S do_mem_mapping(PAGE_DIR *p_page_dir, INT8U present, INT32U priv, INT32U linear_addr, INT32U physical_addr);
void *linear_to_physical(PAGE_DIR *p_page_dir, INT32U linear_addr);
void load_page_dir(PAGE_DIR *p_page_dir);
void enable_paging();
void system_page_init();

#endif
