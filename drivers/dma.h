/*
 *****************************************************************************
 * My Stupid Kernel
 *
 *
 * File Name:   dma.h
 * File Desc:   DMA(Direct Memory Access) driving function
 * Author:      Y.S.Kuo
 * Date:        20130119
 *****************************************************************************
 */


#ifndef _DMA_H_
#define _DMA_H_

#include "../types.h"

void dma_read(INT8U channel, INT32U mem_addr, INT16U mem_size);
void dma_write(INT8U channel, INT32U mem_addr, INT16U mem_size);

#endif
