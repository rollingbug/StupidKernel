/*
 *****************************************************************************
 * My Stupid Kernel
 *
 *
 * File Name:   dma.c
 * File Desc:   DMA(Direct Memory Access) driving function
 * Author:      Y.S.Kuo
 * Date:        20130119
 *****************************************************************************
 */

#include "dma.h"
#include "../kernel.h"

#define DMA_MODE_WRITE      0x04    /* WRITE mean we wanna 'read' data from memory by DMA */
#define DMA_MODE_READ       0x08    /* READ mean we wanna 'write' data to memory by DMA */
#define DMA_MODE_AUTO       0x10
#define DMA_MODE_DOWN       0x20
#define DMA_MODE_DEMAND     0x00
#define DMA_MODE_SINGLE     0x40
#define DMA_MODE_BLOCK      0x80
#define DMA_MODE_CASCADE    0xC0

static INT8U dma_mask_reg[] = {0x0A, 0x0A, 0x0A, 0x0A, 0xD4, 0xD4, 0xD4, 0xD4};
static INT8U dma_ff_reg[] = {0x0C, 0x0C, 0x0C, 0x0C, 0xD8, 0xD8, 0xD8, 0xD8};
static INT8U dma_start_addr_reg[] = {0x00, 0x02, 0x04, 0x06, 0xC0, 0xC4, 0xC8, 0xCC};
static INT8U dma_count_reg[] = {0x01, 0x03, 0x05, 0x07, 0xC2, 0xC6, 0xCA, 0xCE};
static INT8U dma_page_addr_reg[] = {0x87, 0x83, 0x81, 0x82, 0x8F, 0x8B, 0x89, 0x8A};
static INT8U dma_mode_reg[] = {0x0B, 0x0B, 0x0B, 0x0B, 0xD6, 0xD6, 0xD6, 0xD6};

static void dma_setup(INT8U channel, INT32U mem_addr, INT16U mem_size, INT8U mode)
{
    /* mask DMA channel */
    outb(dma_mask_reg[channel], (0x04 | channel));

    /* reset flip-flop and setup DMA memory address (0~15) */
    outb(dma_ff_reg[channel], 0xFF);
    outb(dma_start_addr_reg[channel], (mem_addr & 0x000000FF));
    outb(dma_start_addr_reg[channel], ((mem_addr >> 8) & 0x000000FF));

    /* reset flip-flop and setup DMA memory size */
    outb(dma_ff_reg[channel], 0xFF);
    outb(dma_count_reg[channel], (mem_size & 0x00FF));
    outb(dma_count_reg[channel], ((mem_size >> 8) & 0x00FF));

    /* setup DMA memory page address (16~23) */
    outb(dma_page_addr_reg[channel], ((mem_addr >> 16) & 0x000000FF));

    /* setup DMA mode */
    outb(dma_mode_reg[channel], mode);

    /* unmask DMA channel */
    outb(dma_mask_reg[channel], channel);
}

/* read data by DMA */
void dma_read(INT8U channel, INT32U mem_addr, INT16U mem_size)
{
    INT8U mode;

    channel &= 0x03;
    mem_size -= 1;
    mode = DMA_MODE_SINGLE | DMA_MODE_AUTO | DMA_MODE_WRITE | channel;

    dma_setup(channel, mem_addr, mem_size, mode);
}

/* write data by DMA */
void dma_write(INT8U channel, INT32U mem_addr, INT16U mem_size)
{
    INT8U mode;

    channel &= 0x03;
    mem_size -= 1;
    mode = DMA_MODE_SINGLE | DMA_MODE_AUTO | DMA_MODE_READ | channel;

    dma_setup(channel, mem_addr, mem_size, mode);
}
