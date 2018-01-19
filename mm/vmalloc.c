/*
 *****************************************************************************
 * vmalloc.c (Kernel Virtual Memory Allocate Manager)
 * shiuan 20110723
 *****************************************************************************
 */

#include "vmalloc.h"
#include "mm.h"
#include "../kernel.h"

static VMEM_BLOCK *p_vmem_block_head;
static INT32U system_heap_start = SYSTEM_HEAP_BASE;
extern PAGE_DIR *p_system_page_dir;

static INT32U mem_size_align(INT32U size)
{
    INT32U require_page_num;

    /* align to memory paging size */
    require_page_num = (size / PAGE_MEM_SIZE);
    if(size % PAGE_MEM_SIZE != 0)
        require_page_num += 1;

    return (require_page_num * PAGE_MEM_SIZE);
}

/* scan exist virtual memory block */
static VMEM_BLOCK *get_adapt_block(INT32U require_page_num)
{
    VMEM_BLOCK *p_block;
    VMEM_BLOCK *p_new_block;

    if(require_page_num == 0)
        return NULL;

    p_block = p_vmem_block_head;
    while(p_block != NULL){
        if(p_block->is_free == FALSE || p_block->alloc_page_num < require_page_num)
            p_block = p_block->p_next;
        else
            break;
    }

    /* get apapt block */
    if(p_block != NULL){
        if(p_block->alloc_page_num == require_page_num)
            return p_block;
        /* adjust block allocating */
        else{
            p_new_block = (VMEM_BLOCK *)kmalloc(sizeof(VMEM_BLOCK));
            if(p_new_block == NULL)
                return NULL;

            p_block->alloc_page_num -= require_page_num;

            p_new_block->is_free = FALSE;
            p_new_block->alloc_page_num = require_page_num;
            p_new_block->p_start_addr = (p_block->p_start_addr + (p_block->alloc_page_num * PAGE_MEM_SIZE));
            p_new_block->p_next = p_block->p_next;

            p_block->p_next = p_new_block;

            return p_new_block;
        }
    }

    return NULL;
}

/* this is a very simple virtual allocte function for system kernel,
   function get free memory page by alloc_mem_page function,
   and just increase system heap start address (start from 0xC0000000 when initial) */
void *vmalloc(INT32U require_size)
{
    INT8U alloc_fail;
    INT32U require_page_num;
    INT32U alloc_page_num;
    INT32U tmp_alloc_addr;
    void *p_tmp_page_addr;
    void *p_new_alloc_page;
    VMEM_BLOCK *p_vmem_block;

    require_size = mem_size_align(require_size);

    if(system_heap_start < SYSTEM_HEAP_BASE)
        return NULL;

    /* require size must bigger than zero and smaller than remain virtual memory size */
    if(require_size == 0)
        return NULL;

    require_page_num = require_size / PAGE_MEM_SIZE;
    alloc_page_num = 0;

    /* scan adaptive allocated memory block */
    p_vmem_block = get_adapt_block(require_page_num);

    /* if we cannot get the adaptive memory block, then try to add new one */
    if(p_vmem_block == NULL){
        if(require_size > (SYSTEM_HEAP_END - system_heap_start + 1))
            return NULL;

        p_vmem_block = (VMEM_BLOCK *)kmalloc(sizeof(VMEM_BLOCK));
        if(p_vmem_block == NULL)
            return NULL;

        p_vmem_block->p_next = p_vmem_block_head;
        p_vmem_block_head = p_vmem_block;
        p_vmem_block->p_start_addr = (void *)system_heap_start;
        p_vmem_block->alloc_page_num = require_page_num;
        system_heap_start += require_size;
    }

    /* now start to allocate physical memory and make it become virtual memory */
    alloc_fail = FALSE;
    tmp_alloc_addr = (INT32U)p_vmem_block->p_start_addr;
    while(alloc_page_num < require_page_num){
        /* allocate memory page */
        p_new_alloc_page = (void *)alloc_mem_page();

        /* if allocate fail, stop virtual memory alocate loop */
        if(p_new_alloc_page == NULL){
            alloc_fail = TRUE;
            break;
        }

        /* setup memory mapping, if fail, release latest allocate memory and stop */
        if(do_mem_mapping(p_system_page_dir, TRUE, PAGE_SUPER_PRIV, tmp_alloc_addr, (INT32U)p_new_alloc_page) == -1){
            alloc_fail = TRUE;
            free_mem_page(p_new_alloc_page);
        }

        alloc_page_num++;
        tmp_alloc_addr += PAGE_MEM_SIZE;
    }

    /* if allocate fail, then release all allocated memory and disable virtaul memory mapping */
    if(alloc_fail == TRUE){
        while(alloc_page_num > 1){
            p_tmp_page_addr = linear_to_physical(p_system_page_dir,
                                                 ((INT32U)p_vmem_block->p_start_addr) + ((alloc_page_num - 2) * PAGE_MEM_SIZE));

            do_mem_mapping(p_system_page_dir,
                           FALSE,
                           PAGE_SUPER_PRIV,
                           ((INT32U)p_vmem_block->p_start_addr) + ((alloc_page_num - 2) * PAGE_MEM_SIZE),
                           (INT32U)NULL);

            free_mem_page(p_tmp_page_addr);
            alloc_page_num--;
        }

        p_vmem_block->is_free = TRUE;
        return NULL;
    }
    /* if success, setup block to "non free" and return the virtual memory start address. */
    else{
        p_vmem_block->is_free = FALSE;
        return p_vmem_block->p_start_addr;
    }
}

void vfree(void *p_mem_addr)
{
    VMEM_BLOCK *p_tmp_block;
    INT32U mem_offset;
    void *p_tmp_page_addr;

    if(p_mem_addr == NULL)
        return;

    /* now start to scan virtual memory block */
    p_tmp_block = p_vmem_block_head;
    mem_offset = 0;
    while(p_tmp_block != NULL){

        /* if get the match block, then release all physical memory and disable virtual memory mapping */
        if(p_tmp_block->p_start_addr == p_mem_addr){
            while(mem_offset < (p_tmp_block->alloc_page_num * PAGE_MEM_SIZE)){
                p_tmp_page_addr = linear_to_physical(p_system_page_dir, (INT32U)(p_tmp_block->p_start_addr) + mem_offset);
                do_mem_mapping(p_system_page_dir, FALSE, PAGE_SUPER_PRIV, (INT32U)(p_tmp_block->p_start_addr) + mem_offset, (INT32U)NULL);
                free_mem_page(p_tmp_page_addr);

                mem_offset += PAGE_MEM_SIZE;
            }

            /* setup match memory block to "free" */
            p_tmp_block->is_free = TRUE;
        }

        p_tmp_block = p_tmp_block->p_next;
    }
}

void vmalloc_init()
{
    p_vmem_block_head = NULL;

    /* system heap address start from 0xC0000000 */
    system_heap_start = SYSTEM_HEAP_BASE;

    KERNEL_MSG(FALSE, TRUE, "vmalloc is ready.\n");
}
