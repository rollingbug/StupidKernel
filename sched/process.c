/*
 *****************************************************************************
 * process.c (Process Management)
 * shiuan 20110116
 *****************************************************************************
 */

#include "process.h"
#include "sched.h"

INT16U process_id_cnt;

INT8U process_setup_user_stk(PAGE_DIR *p_user_page_dir, INT32U stk_size)
{
    INT32U require_mem_page;
    INT32U stk_mem_start;
    INT32U alloc_page_cnt;
    INT32U alloc_fail;
    void *p_physical_mem_addr;

    if(p_user_page_dir == NULL || stk_size == 0)
        return -1;

    stk_mem_start = DEF_USER_STK_BASE;
    alloc_page_cnt = 0;
    alloc_fail = FALSE;
    while(stk_size != 0){
        p_physical_mem_addr = alloc_mem_page();
        if(p_physical_mem_addr == NULL){
            alloc_fail = TRUE;
            break;
        }

        if(do_mem_mapping(p_user_page_dir, TRUE, PAGE_SUPER_PRIV, stk_mem_start, (INT32U)p_physical_mem_addr) == -1){
            free_mem_page(p_physical_mem_addr);
            alloc_fail = TRUE;
            break;
        }

        alloc_page_cnt++;
        stk_mem_start += PAGE_MEM_SIZE;

        if(stk_size < PAGE_MEM_SIZE)
            stk_size = 0;
        else
            stk_size -= PAGE_MEM_SIZE;
    }

    if(alloc_fail == TRUE){
        stk_mem_start = DEF_USER_STK_BASE;
        while(alloc_page_cnt > 0){

            p_physical_mem_addr = linear_to_physical(p_user_page_dir,
                                  (stk_mem_start + (alloc_page_cnt - 1) * PAGE_MEM_SIZE));

            if(p_physical_mem_addr != NULL)
                free_mem_page(p_physical_mem_addr);

            alloc_page_cnt--;
        }

        return -1;
    }
    else
        return 0;
}

PROCESS_INFO *process_create(PROCESS_CREATE_INFO *p_create_info)
{
    PROCESS_INFO *p_new_process;

    if(p_create_info == NULL)
        return NULL;

    if(p_create_info->code_size == 0)
        return NULL;

    if(p_create_info->p_mem_page_dir == NULL)
        return NULL;

    p_new_process = (PROCESS_INFO *)alloc_mem_page();

    if(p_new_process == NULL)
        return NULL;

    memset((void *)p_new_process, 0, sizeof(PROCESS_INFO));
    memcpy((void *)&(p_new_process->process_create_info), p_create_info, sizeof(PROCESS_CREATE_INFO));

    /* setup basic process information */
    p_new_process->pid = process_id_cnt++;
    p_new_process->process_level = PROCESS_USER_LEVEL;
    p_new_process->kernel_stack_size = DEF_KERNEL_STK_SIZE;
    p_new_process->p_kernel_stack_start = (vmalloc(DEF_KERNEL_STK_SIZE) + DEF_KERNEL_STK_SIZE - 1);

    /* now try to allocate user process stack */
    if(process_setup_user_stk(p_new_process->process_create_info.p_mem_page_dir, DEF_USER_STK_SIZE) == -1){
        vfree((void *)p_new_process->p_kernel_stack_start);
        vfree((void *)p_new_process);
        return NULL;
    }

    p_new_process->user_stack_size = DEF_USER_STK_SIZE;
    p_new_process->p_user_stack_start = (void *)(DEF_USER_STK_BASE + DEF_USER_STK_SIZE - 1);
    p_new_process->console_buf_idx = 0;
    p_new_process->keyboard_buf_idx = 0;
    p_new_process->p_next = NULL;

    /* initial cpu task state segment(TSS) */
    p_new_process->tss.prev_task_link = 0;
    p_new_process->tss.esp0 = (INT32U)alloc_mem_page() + 4095;
    p_new_process->tss.ss0 = KERNEL_DATA_SEL;
    p_new_process->tss.esp1 = 0;
    p_new_process->tss.ss1 = 0;
    p_new_process->tss.esp2 = 0;
    p_new_process->tss.ss2 = 0;

    /* user process memory paging dir */
    p_new_process->tss.cr3 = (INT32U)p_create_info->p_mem_page_dir;

    /* setup code, data, and stack segment of process */
    p_new_process->tss.eip = (INT32U)p_create_info->p_process_code_entry;
    p_new_process->tss.eflags = 0x0200;
    p_new_process->tss.eax = 0;
    p_new_process->tss.ecx = 0;
    p_new_process->tss.edx = 0;
    p_new_process->tss.ebx = 0;
    p_new_process->tss.esp = (INT32U)alloc_mem_page() + 4095;
    p_new_process->tss.ebp = 0;
    p_new_process->tss.esi = 0;
    p_new_process->tss.edi = 0;
    p_new_process->tss.es = USER_DATA_SEL | 3;
    p_new_process->tss.cs = USER_CODE_SEL | 3;
    p_new_process->tss.ss = USER_DATA_SEL | 3;
    p_new_process->tss.ds = USER_DATA_SEL | 3;
    p_new_process->tss.fs = USER_DATA_SEL | 3;
    p_new_process->tss.gs = USER_DATA_SEL | 3;
    p_new_process->tss.ldt = 0;
    p_new_process->tss.io_map_base = 0x0800;

    return p_new_process;
}

INT8S process_delete(PROCESS_INFO *p_process_info)
{

}

void process_init()
{
    process_id_cnt = 0;
}
