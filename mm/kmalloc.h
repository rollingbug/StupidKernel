/*
 *****************************************************************************
 * kmalloc.h (Kernel Memory Allocate Manager)
 * shiuan 20110111
 *****************************************************************************
 */

#ifndef _KMALLOC_H_
#define _KMALLOC_H_

#include "../types.h"

typedef struct block_node{
    void *page_addr;
    INT32U free_block_idx;
    INT32U free_block_num;
    struct block_node *next_node;
}__attribute__((packed))BLOCK_NODE;

typedef struct block_entry{
    INT32U block_size;
    INT32U block_total;
    struct block_node *node_head;
}__attribute__((packed))BLOCK_ENTRY;

#define get_block_addr(block_size, block_idx) \
        ((p_node->free_block_idx - 1) * block_size) + sizeof(BLOCK_NODE)

#define get_block_index(block_size, page_addr, block_addr) \
        ((block_addr - page_addr - sizeof(BLOCK_NODE)) / block_size) + 1

#endif
