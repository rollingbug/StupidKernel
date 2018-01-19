/*
 *****************************************************************************
 * mm.c (Memory Management)
 * shiuan 20101226
 *****************************************************************************
 */

#include "mm.h"
#include "physical.h"
#include "segment.h"
#include "paging.h"
#include "kmalloc.h"
#include "vmalloc.h"
#include "../drivers/dma.h"
#include "../kernel.h"

void mm_init()
{
    extern INT32U kernel_end;
    INT32U free_mem_start;

    free_mem_start = (INT32U)&kernel_end;

    /* initial physical memory page table
       (calculate there are how many free memory for kernel and user)
       after physical memory has been initialized,
       that mean the physical memory begin to managed by kernel */
	physical_mem_init(free_mem_start);

    /* initial system segment */
    system_segment_init();

	/* initial kernel page and enable x86 cpu paging */
	system_page_init();

    /* ok ... now we know the physical memory size and free memory size */
    KERNEL_MSG(FALSE, TRUE, "physical memory = %uK, free memroy = %uK\n",
               get_physical_mem_size() / 1024, get_free_mem_size() / 1024);

    /* initial kernel memory allocator */
    kmalloc_init();

    /* initial kernel virtual memory allocator */
    vmalloc_init();
}


