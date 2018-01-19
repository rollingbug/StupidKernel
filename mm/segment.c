/*
 *****************************************************************************
 * segment.c (x86 Memory Segment Manager)
 * shiuan 20110111
 *****************************************************************************
 */

#include "segment.h"
#include "../kernel.h"

/* segment descriptor */
SEG_R system_seg_r;
SEG_DESC system_seg_desc[SYSTEM_SEG_NUM];

INT8S setup_seg_desc(INT32U seg_sel, INT32U base, INT32U limit, INT8U desc_type, INT8U dpl, INT8U seg_type)
{
    seg_sel /= 8;
    if(seg_sel > SYSTEM_SEG_NUM - 1)
        return -1;

    system_seg_desc[seg_sel].limit_low = limit & 0xFFFF;
    system_seg_desc[seg_sel].base_low = base & 0xFFFF;
    system_seg_desc[seg_sel].base_mid = (base & 0xFF0000) >> 16;
    system_seg_desc[seg_sel].seg_type = seg_type;
    system_seg_desc[seg_sel].desc_type = desc_type;
    system_seg_desc[seg_sel].dpl = dpl;
    system_seg_desc[seg_sel].present = 1;
    system_seg_desc[seg_sel].limit_high = (limit & 0xF0000) >> 16;
    system_seg_desc[seg_sel].avl = 0;
    system_seg_desc[seg_sel].zero = 0;
    system_seg_desc[seg_sel].operation_size = SEG_SIZE_32BITS;
    system_seg_desc[seg_sel].granularity = 1;
    system_seg_desc[seg_sel].base_high = (base & 0xFF000000) >> 24;

    return 0;
}

void load_tss(INT16U tss_sel)
{
    asm("ltr %0"
        :
        :"r" (tss_sel)
    );
}

void load_segment()
{
    /* load GDT and update cpu registers */
    asm("lgdt (%0);         	\
         ljmp %1, $new_seg;    	\
         new_seg:;             	\
         movw %2, %%ax;     	\
         movw %%ax, %%ds;   	\
         movw %%ax, %%ss;   	\
         movw %%ax, %%es;   	\
         movw %%ax, %%fs;   	\
         movw %%ax, %%gs;"
        :
        : "r" (&system_seg_r), "i" (KERNEL_CODE_SEL), "i" (KERNEL_DATA_SEL)
    );
}

void system_seg_init()
{
    memset((void *)system_seg_desc, 0, sizeof(SEG_DESC) * SYSTEM_SEG_NUM);

    /* code segment for kernel, base = 0x0, limit = 4G */
    setup_seg_desc(KERNEL_CODE_SEL, 0x0, 0xFFFFF, DESC_TYPE_CODEDATA, KERNEL_PRIV_KERNEL, SEG_TYPE_CODE_ONLY);

    /* data segment for kernel, base = 0x0, limit = 4G */
    setup_seg_desc(KERNEL_DATA_SEL, 0x0, 0xFFFFF, DESC_TYPE_CODEDATA, KERNEL_PRIV_KERNEL, SEG_TYPE_DATA_ONLY);

    /* code segment for user, base = 0x0, limit = 4G */
    setup_seg_desc(USER_CODE_SEL, 0x0, 0xFFFFF, DESC_TYPE_CODEDATA, KERNEL_PRIV_USER, SEG_TYPE_CODE_ONLY);

    /* data segment for user, base = 0x0, limit = 4G */
    setup_seg_desc(USER_DATA_SEL, 0x0, 0xFFFFF, DESC_TYPE_CODEDATA, KERNEL_PRIV_USER, SEG_TYPE_DATA_ONLY);
}

void system_segment_init()
{
    system_seg_r.limit = (sizeof(SEG_DESC) * SYSTEM_SEG_NUM) - 1;
    system_seg_r.base = (INT32U)system_seg_desc;

    system_seg_init();
    load_segment();
}
