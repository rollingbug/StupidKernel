/*
 *****************************************************************************
 * process.h (Process Management)
 * shiuan 20110116
 *****************************************************************************
 */

#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "../types.h"
#include "../mm/mm.h"

#define PROCESS_KERNEL_LEVEL    0x00
#define PROCESS_USER_LEVEL      0x01

#define DEF_KERNEL_STK_SIZE     4096
#define DEF_KERNEL_STK_START

#define DEF_USER_STK_SIZE       4096
#define DEF_USER_STK_BASE       0x1000000       /* start from 1MB memory */

#define DEF_USER_CODE_START     0x40000000      /* start from 1GB memory */
#define DEF_USER_DATA_START     0x80000000      /* start from 2GB memory */

typedef struct process_create_info{
    INT8U *p_process_name;
    INT8U *p_process_file_path;
    void *p_process_code_entry;
    PAGE_DIR *p_mem_page_dir;
    INT32U code_size;
    INT32U data_size;
    INT8U process_type;
}__attribute__((packed))PROCESS_CREATE_INFO;

typedef struct process_info{
    PROCESS_CREATE_INFO process_create_info;
    INT16U pid;
    INT8U process_level;
    void *p_user_stack_start;
    void *p_kernel_stack_start;
    INT32U user_stack_size;
    INT32U kernel_stack_size;
    INT8U *p_keyboard_buf;
    INT32U keyboard_buf_idx;
    INT8U *p_console_buf;
    INT32U console_buf_idx;
    TSS_DESC tss;
    struct process_info *p_next;
}__attribute__((packed))PROCESS_INFO;

#endif
