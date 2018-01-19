/*
 *****************************************************************************
 * kmalloc.c (Kernel Memory Allocate Manager)
 * shiuan 20110111
 *****************************************************************************
 */

#include "kmalloc.h"
#include "mm.h"
#include "../kernel.h"

INT32U max_block_size;

BLOCK_ENTRY block_entry[] =
{
    {32,    127,    NULL},
    {64,    63,     NULL},
    {128,   31,     NULL},
    {256,   15,     NULL},
    {510,   8,      NULL},
    {1020,  4,      NULL},
    {2040,  2,      NULL},
    {4080,  1,      NULL},
    {0,     0,      NULL}
};

INT8S get_entry_index(INT32U size)
{
    INT8U index;

    index = 0;
    while(block_entry[index].block_size != 0){
        if(size <= block_entry[index].block_size)
            return index;

        index++;
    }

    return -1;
}

INT32U get_max_block_size()
{
    return max_block_size;
}

void *kmalloc(INT32U size)
{
    INT8U index;
    BLOCK_NODE *p_node;
    BLOCK_NODE *p_prev_node;
    INT32U *block_addr;
    INT32U block_count;
    void *mem_addr;

    if(size == 0 || size > max_block_size)
        return NULL;

    index = get_entry_index(size);
    if(index == -1)
        return NULL;

    /* now try to allocate a free memroy block for user form exist blocks */
    p_node = block_entry[index].node_head;
    p_prev_node = NULL;
    while(p_node != NULL){
        if(p_node->free_block_idx != 0){
            p_node->free_block_num--;
            mem_addr = p_node->page_addr + get_block_addr(block_entry[index].block_size,
                                                            p_node->free_block_idx);
            p_node->free_block_idx = *((INT32U *)(mem_addr));

            return mem_addr;    /* we get free memory block */
        }

        p_prev_node = p_node;
        p_node = p_node->next_node;
    }

    /* if we dont have availabel blocks, try to allocate new memory page */
    p_node = (BLOCK_NODE *)alloc_mem_page();
    if(p_node == NULL)
        return NULL;

    p_node->free_block_idx = 2; /* free block index start from 2, because the first free block will allocate to current user */
    p_node->free_block_num = block_entry[index].block_total - 1;
    p_node->next_node = NULL;
    p_node->page_addr = (void *)p_node;     /* i think this is not really necessary */

    /* setup index in each blocks */
    block_addr = (INT32U *)(p_node->page_addr + sizeof(BLOCK_NODE));
    block_count = 1;
    while(block_count < block_entry[index].block_total){
        *block_addr = block_count + 1;
        block_addr = (void *)block_addr + block_entry[index].block_size;
        block_count++;
    }

    *block_addr = 0;    /* the last one block index should be zero */

    /* link the list */
    if(p_prev_node != NULL)
        p_prev_node->next_node = p_node;
    else
        block_entry[index].node_head = p_node;

    return (p_node->page_addr + sizeof(BLOCK_NODE));
}

void kfree(void *addr)
{
    INT8U index;
    BLOCK_NODE *p_node;
    BLOCK_NODE *p_prev_node;

    if(addr == NULL)
        return;

    /* scan every block node */
    index = 0;
    while(block_entry[index].block_size != 0){
        p_node = block_entry[index].node_head;
        p_prev_node = NULL;
        while(p_node != NULL){
            if(p_node->page_addr < addr && p_node->page_addr + PAGE_MEM_SIZE > addr){
                *((INT32U *)addr) = p_node->free_block_idx;
                p_node->free_block_idx = get_block_index(block_entry[index].block_size,
                                                         p_node->page_addr,
                                                         addr);
                p_node->free_block_num++;
                if(p_node->free_block_num == block_entry[index].block_total){
                    if(p_prev_node == NULL)
                        block_entry[index].node_head = p_node->next_node;
                    else
                        p_prev_node->next_node = p_node->next_node;

                    /* free memory page */
                    free_mem_page(p_node->page_addr);
                }

                return;
            }

            p_node = p_node->next_node;
        }

        index++;
    }
}

kmalloc_init()
{
    INT8U index;

    KERNEL_MSG(FALSE, TRUE, "block size(bytes) : ");

    max_block_size = 0;
    index = 0;
    while(block_entry[index].block_size != 0){
        KERNEL_MSG(FALSE, FALSE, "%d", block_entry[index].block_size);

		if(block_entry[index + 1].block_size != 0)
			KERNEL_MSG(FALSE, FALSE, ", ");

        if(max_block_size < block_entry[index].block_size)
            max_block_size = block_entry[index].block_size;

        index++;
    }

    KERNEL_MSG(FALSE, FALSE, "\n");
}
