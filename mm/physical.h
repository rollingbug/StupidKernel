/*
 *****************************************************************************
 * physical.h (Physical Memory Management)
 * shiuan 20110102
 *****************************************************************************
 */

#ifndef _PHYSICAL_H_
#define _PHYSICAL_H_

#include "../types.h"

#define PHYSICAL_LOW_MEM    (1024 * 1024 * 16)

INT32U get_physical_mem_size();
INT32U get_free_mem_size();
void *alloc_physical_mem_page(INT32U mem_limit_start, INT32U mem_limit_end);
void *alloc_low_mem_page();
void *alloc_high_mem_page();
void *alloc_mem_page();
void free_mem_page(void *mem_addr);
void physical_mem_init(INT32U free_mem_start);

#endif
