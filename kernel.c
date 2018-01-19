/*
 *****************************************************************************
 * My Stupid Kernel
 *
 *
 * File Name:   kernel.c
 * File Desc:   main functions of kernel
 * Author:      Y.S.Kuo
 * Date:        20130119
 *****************************************************************************
 */
#include "kernel.h"
#include "lib/lib.h"
#include "console/console.h"
#include "interrupt/interrupt.h"
#include "mm/mm.h"
#include "sched/sched.h"
#include "drivers/floppy.h"
#include "drivers/keyboard.h"
#include "fs/fat12.h"

void kernel_halt()
{
    asm("hlt");
}

void kernel_panic(INT8S *panic_msg)
{
	console_printf("\n[kernel panic!] %s", panic_msg);
	interrupt_disable();
	kernel_halt();
}

void tss_code1()
{
    INT32U i;

    while(1){
        i = 0;
        while(i++ < 1000000);

        console_printf("task1 ");
    }
}

void tss_code2()
{
    INT32U i;

    while(1){
        i = 0;
        while(i++ < 1000000);

        console_printf("task2 ");
    }
}

void kernel_main()
{
    interrupt_disable();

	console_init();
	interrupt_init();
	mm_init();
	sched_init();
	floppy_init();
	keyboard_init();
	fat12_init();

	interrupt_enable();



/* new task test code */
/*
	PROCESS_INFO *p_process_info;
	PROCESS_CREATE_INFO *p_create_info;
	PAGE_DIR *p_page_dir;
	INT32U phy_addr;

	p_page_dir = (PAGE_DIR *)alloc_mem_page();
	memset((void *)p_page_dir, 0, sizeof(PAGE_DIR));

	p_create_info = (PROCESS_CREATE_INFO *)vmalloc(sizeof(PROCESS_CREATE_INFO));
	p_create_info->code_size = 1;
	p_create_info->data_size = 1;
	p_create_info->process_type = 1;
	p_create_info->p_process_code_entry = tss_code2;
	p_create_info->p_process_name = NULL;
	p_create_info->p_process_file_path = NULL;
	p_create_info->p_mem_page_dir = p_page_dir;

    phy_addr = 0;
    while(phy_addr < get_physical_mem_size() - 1){
        do_mem_mapping(p_page_dir, TRUE, PAGE_SUPER_PRIV, (INT32U)phy_addr, (INT32U)phy_addr);
        phy_addr += 4096;
    }

    p_process_info = (PROCESS_INFO *)process_create(p_create_info);
    if(p_process_info == NULL){
        kprintf("fail\n");
        while(1);
    }

    add_process(p_process_info);
    schedule();
*/

///*************************************************************************/
// old task test code
///*************************************************************************/

/*
    PROCESS_INFO *p_tss1;
    PROCESS_INFO p_tss2;
    PAGE_DIR *p_page_dir;
    PAGE_TABLE *p_page_table;
    INT32U phy_addr;


    p_tss1 = (PROCESS_INFO *)alloc_mem_page();

    memset((void *)p_tss1, 0, sizeof(PROCESS_INFO));
    memset((void *)&p_tss1->tss, 0xFFFFFFFF, sizeof(TSS_DESC));

    p_tss1->pid = 1;
    p_tss1->p_next = NULL;

    p_tss1->tss.prev_task_link = 0;
    p_tss1->tss.esp0 = (INT32U)alloc_mem_page() + 4095;
    p_tss1->tss.ss0 = KERNEL_DATA_SEL;
    p_tss1->tss.esp1 = 0;
    p_tss1->tss.ss1 = 0;
    p_tss1->tss.esp2 = 0;
    p_tss1->tss.ss2 = 0;

    p_page_dir = (PAGE_DIR *)alloc_mem_page();

    memset((void *)p_page_dir, 0, sizeof(PAGE_DIR));

    phy_addr = 0;
    while(phy_addr < get_physical_mem_size() - 1){
        if(phy_addr % (4096 * 1024) == 0){
            p_page_table = (PAGE_TABLE *)alloc_mem_page();

            memset((void *)p_page_table, 0, sizeof(PAGE_TABLE));

            setup_page_dir(p_page_dir, phy_addr, PAGE_PRESENT,
                           PAGE_READWRITE, PAGE_SUPER_PRIV, p_page_table);
        }

        setup_page_table(p_page_dir, phy_addr,PAGE_PRESENT,
                         PAGE_READWRITE, PAGE_SUPER_PRIV, phy_addr);

        phy_addr += 4096;
    }

    p_tss1->tss.cr3 = (INT32U)p_page_dir;

    p_tss1->tss.eip = (INT32U)tss_code1;
    p_tss1->tss.eflags = 0x0200;
    p_tss1->tss.eax = 0;
    p_tss1->tss.ecx = 0;
    p_tss1->tss.edx = 0;
    p_tss1->tss.ebx = 0;
    p_tss1->tss.esp = (INT32U)alloc_mem_page() + 4095;
    p_tss1->tss.ebp = 0;
    p_tss1->tss.esi = 0;
    p_tss1->tss.edi = 0;
    p_tss1->tss.es = USER_DATA_SEL | 3;
    p_tss1->tss.cs = USER_CODE_SEL | 3;
    p_tss1->tss.ss = USER_DATA_SEL | 3;
    p_tss1->tss.ds = USER_DATA_SEL | 3;
    p_tss1->tss.fs = USER_DATA_SEL | 3;
    p_tss1->tss.gs = USER_DATA_SEL | 3;
    p_tss1->tss.ldt = 0;
    //p_tss1->tss.io_map_base = 0x0800;
    p_tss1->tss.io_map_base = (sizeof(TSS_DESC) - (2048 * 4));
*/

//
///*************************************************************************/
//    memset((void *)&p_tss2, 0, sizeof(PROCESS));
//
//    p_tss2.pid = 2;
//    p_tss2.next = NULL;
//
//    p_tss2.tss.prev_task_link = 0;
//    p_tss2.tss.esp0 = (INT32U)alloc_mem_page() + 4095;
//    p_tss2.tss.ss0 = KERNEL_DATA_SEL;
//    p_tss2.tss.esp1 = 0;
//    p_tss2.tss.ss1 = 0;
//    p_tss2.tss.esp2 = 0;
//    p_tss2.tss.ss2 = 0;
//
//    p_page_dir = (PAGE_DIR *)alloc_mem_page();
//    phy_addr = 0;
//    while(phy_addr < get_physical_mem_size() - 1){
//        if(phy_addr % (4096 * 1024) == 0){
//            p_page_table = (PAGE_TABLE *)alloc_mem_page();
//
//            memset((void *)p_page_table, 0, sizeof(PAGE_TABLE));
//
//            setup_page_dir(p_page_dir, phy_addr, PAGE_PRESENT,
//                           PAGE_READWRITE, PAGE_SUPER_PRIV, p_page_table);
//        }
//
//        setup_page_table(p_page_dir, phy_addr,PAGE_PRESENT,
//                         PAGE_READWRITE, PAGE_SUPER_PRIV, phy_addr);
//
//        phy_addr += 4096;
//    }
//
//    p_tss2.tss.cr3 = (INT32U)p_page_dir;
//
//    p_tss2.tss.eip = (INT32U)tss_code2;
//    p_tss2.tss.eflags = 0x0200;
//    p_tss2.tss.eax = 0;
//    p_tss2.tss.ecx = 0;
//    p_tss2.tss.edx = 0;
//    p_tss2.tss.ebx = 0;
//    p_tss2.tss.esp = (INT32U)alloc_mem_page() + 4095;
//    p_tss2.tss.ebp = 0;
//    p_tss2.tss.esi = 0;
//    p_tss2.tss.edi = 0;
//    p_tss2.tss.es = USER_DATA_SEL | 3;
//    p_tss2.tss.cs = USER_CODE_SEL | 3;
//    p_tss2.tss.ss = USER_DATA_SEL | 3;
//    p_tss2.tss.ds = USER_DATA_SEL | 3;
//    p_tss2.tss.fs = USER_DATA_SEL | 3;
//    p_tss2.tss.gs = USER_DATA_SEL | 3;
//    p_tss2.tss.ldt = 0;
//    p_tss2.tss.io_map_base = 0x0800;
//
//
//
//
//
//add_process(p_tss1);
//add_process(&p_tss2);
//enable_interrupt();




//schedule();

    //switch_to(USER_TSS_SEL);
/*
    TSS_DESC tss;
    PAGE_DIR *p_page_dir;
    PAGE_TABLE *p_page_table;
    INT32U phy_addr;
    memset((void *)&tss, 0, sizeof(TSS_DESC));

    tss.prev_task_link = 0;
    tss.esp0 = (INT32U)alloc_physical_mem_page() + 4095;
    tss.ss0 = KERNEL_DATA_SEL;
    tss.esp1 = 0;
    tss.ss1 = 0;
    tss.esp2 = 0;
    tss.ss2 = 0;

    p_page_dir = (PAGE_DIR *)alloc_physical_mem_page();

    tss.cr3 = (INT32U)p_page_dir;
    phy_addr = 0;
    while(phy_addr < get_physical_mem_size() - 1){
        if(phy_addr % (4096 * 1024) == 0){
            p_page_table = (PAGE_TABLE *)alloc_physical_mem_page();

            memset((void *)p_page_table, 0, sizeof(PAGE_TABLE));

            setup_page_dir(p_page_dir, phy_addr, PAGE_PRESENT,
                           PAGE_READWRITE, PAGE_SUPER_PRIV, p_page_table);
        }

        setup_page_table(p_page_dir, phy_addr,PAGE_PRESENT,
                         PAGE_READWRITE, PAGE_SUPER_PRIV, phy_addr);

        phy_addr += 4096;
    }

    tss.eip = (INT32U)tss_code;
    tss.eflags = 0x0200;
    tss.eax = 0;
    tss.ecx = 0;
    tss.edx = 0;
    tss.ebx = 0;
    tss.esp = (INT32U)alloc_physical_mem_page() + 4095;
    tss.ebp = 0;
    tss.esi = 0;
    tss.edi = 0;
    tss.es = USER_DATA_SEL | 3;
    tss.cs = USER_CODE_SEL | 3;
    tss.ss = USER_DATA_SEL | 3;
    tss.ds = USER_DATA_SEL | 3;
    tss.fs = USER_DATA_SEL | 3;
    tss.gs = USER_DATA_SEL | 3;
    tss.ldt = 0;
    tss.io_map_base = 0x0800;


    setup_seg_desc(USER_TSS_SEL, (INT32U)&tss, sizeof(TSS_DESC), DESC_TYPE_SYSTEM, PRIV_USER, SEG_TYPE_CODE_ACCESS);

    asm volatile("ljmp %0, $fake_label\n\t \
                 fake_label: \n\t"
                 :
                 : "i"(0x28)
    );

*/

	while(1){
		kernel_halt();
	}
}
