/*
 *****************************************************************************
 * sched.h (Scheduler)
 * shiuan 20110116
 *****************************************************************************
 */

#ifndef _SCHED_H_
#define _SCHED_H_

#include "process.h"
#include "../types.h"
#include "../mm/mm.h"

#define switch_to(tss_sel)          \
    asm volatile("ljmp %0, $task;   \
                 task:;"            \
                 :                  \
                 :"i"(tss_sel)      \
    );

INT8S add_process(PROCESS_INFO *p_new_process);
void remove_process(INT16U pid);
INT8S switch_process(PROCESS_INFO *p_process);
void schedule();
void sched_init();

#endif
