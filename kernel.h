/*
 *****************************************************************************
 * My Stupid Kernel
 *
 *
 * File Name:   kernel.h
 * File Desc:   main functions of kernel
 * Author:      Y.S.Kuo
 * Date:        20130119
 *****************************************************************************
 */
#ifndef _KERNEL_H_
#define _KERNEL_H_

#include "types.h"

#define KERNEL_MEM_START_ADDR   0x0                 /* start from 0x00000000 */

#define KERNEL_PRIV_KERNEL      0x0
#define KERNEL_PRIV_USER        0x3

#define KERNEL_MSG(debug_only, func_tag, ...)               \
    if(debug_only != TRUE){                                 \
        if(func_tag == TRUE)                                \
            console_printf("[%s] ", __FUNCTION__);                 \
                                                            \
        console_printf(__VA_ARGS__);                               \
    }

void kernel_panic();

#endif
