/*
 *****************************************************************************
 * vmalloc.h (Kernel Virtual Memory Allocate Manager)
 * shiuan 20110723
 *****************************************************************************
 */

#ifndef _VMALLOC2_H_
#define _VMALLOC2_H_

#include "../types.h"

#define SYSTEM_HEAP_BASE    0xC0000000              /* start from 3G */
#define SYSTEM_HEAP_END     0xFFFFFFFF              /* end to 4G */

typedef struct vmem_block{
    void *p_start_addr;
    INT32U alloc_page_num;
    INT8U is_free;
    struct vmem_block *p_next;
}VMEM_BLOCK;

void *vmalloc(INT32U require_size);
void vfree(void *p_mem_addr);
void vmalloc_init();

#endif
