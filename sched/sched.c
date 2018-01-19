/*
 *****************************************************************************
 * sched.c (Scheduler)
 * shiuan 20110116
 *****************************************************************************
 */

#include "sched.h"
#include "../kernel.h"

PROCESS_INFO *p_process_list;
PROCESS_INFO *p_curt_process;

INT8S add_process(PROCESS_INFO *p_new_process)
{
    PROCESS_INFO *p_process;

    if(p_new_process == NULL)
        return -1;

    if(p_process_list == NULL){
        p_process_list = p_new_process;
        p_curt_process = p_new_process;
    }
    else{
        p_process = p_process_list;
        while(p_process->p_next != NULL)
            p_process = p_process->p_next;

        p_process->p_next = p_new_process;
    }

    return 0;
}

void remove_process(INT16U pid)
{
    PROCESS_INFO *p_process;

    if(p_process_list != NULL){
        if(p_process_list->pid == pid){
            p_process_list = p_process_list->p_next;
            if(p_curt_process->pid == pid)
                p_curt_process == p_process_list;
        }
        else{
            p_process = p_process_list;
            while(p_process->p_next != NULL){
                if(p_process->p_next->pid == pid){
                    p_process->p_next = p_process->p_next->p_next;
                    if(p_curt_process->pid == pid)
                        p_curt_process = p_process->p_next;

                    break;
                }

                p_process = p_process->p_next;
            }
        }
    }

}

INT8S switch_process(PROCESS_INFO *p_process)
{
    if(p_process == NULL)
        return -1;

    /* setup tss segment for user, base = address of tss, limit = sizeof(TSS_DESC) */
    setup_seg_desc(USER_TSS_SEL, (INT32U)&(p_process->tss), sizeof(TSS_DESC), DESC_TYPE_SYSTEM, KERNEL_PRIV_USER, SEG_TYPE_CODE_ACCESS);
    switch_to(USER_TSS_SEL);
}

void schedule()
{
    if(p_process_list != NULL){
        if(p_curt_process != NULL){
            p_curt_process = p_curt_process->p_next;
            if(p_curt_process == NULL)
                p_curt_process = p_process_list;
        }
    }

    if(p_curt_process != NULL)
        switch_process(p_curt_process);
}

void sched_init()
{
    p_process_list = NULL;
    p_curt_process = NULL;
}
