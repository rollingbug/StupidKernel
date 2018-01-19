/*
 *****************************************************************************
 * segment.h (x86 Memory Segment Manager)
 * shiuan 20110111
 *****************************************************************************
 */

#ifndef _SEGMENT_H_
#define _SEGMENT_H_

#include "../types.h"

#define SYSTEM_SEG_NUM          0x06
#define TSS_IO_MAP_NUM          0x800

#define KERNEL_CODE_SEL         0x08
#define KERNEL_DATA_SEL         0x10
#define USER_CODE_SEL           0x18
#define USER_DATA_SEL           0x20
#define USER_TSS_SEL            0x28

#define SEG_TYPE_CODE_ACCESS    0x09
#define SEG_TYPE_CODE_ONLY      0x0A
#define SEG_TYPE_DATA_ONLY      0x02

#define DESC_TYPE_SYSTEM        0x00
#define DESC_TYPE_CODEDATA      0x01

#define SEG_SIZE_16BITS         0x00
#define SEG_SIZE_32BITS         0x01

typedef struct seg_r{
	INT16U limit;				/* segment table size */
	INT32U base;				/* segment table base address */
}__attribute__((packed)) SEG_R;

typedef struct seg_desc{
    INT16U limit_low;
    INT16U base_low;
    INT8U base_mid;
    INT8U seg_type:4;
    INT8U desc_type:1;
    INT8U dpl:2;
    INT8U present:1;
    INT8U limit_high:4;
    INT8U avl:1;
    INT8U zero:1;
    INT8U operation_size:1;
    INT8U granularity:1;
    INT8U base_high;
}__attribute__((packed)) SEG_DESC;

typedef struct tss_desc{
    INT16U prev_task_link, reserved1;
    INT32U esp0;
    INT16U ss0, reserved2;
    INT32U esp1;
    INT16U ss1, reserved3;
    INT32U esp2;
    INT16U ss2, eserved4;
    INT32U cr3;
    INT32U eip;
    INT32U eflags;
    INT32U eax;
    INT32U ecx;
    INT32U edx;
    INT32U ebx;
    INT32U esp;
    INT32U ebp;
    INT32U esi;
    INT32U edi;
    INT16U es, reserved5;
    INT16U cs, reserved6;
    INT16U ss, reserved7;
    INT16U ds, reserved8;
    INT16U fs, reserved9;
    INT16U gs, reserved10;
    INT16U ldt, reserved11;
    INT16U reserved12, io_map_base;
    INT32U io_map[TSS_IO_MAP_NUM];
}__attribute__((packed)) TSS_DESC;

void system_segment_init();

#endif
