/*
 *****************************************************************************
 * My Stupid Kernel
 *
 *
 * File Name:   interrupt.h
 * File Desc:   function to initial and control system interrupt
 * Author:      Y.S.Kuo
 * Date:        20130119
 *****************************************************************************
 */

#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#include "../types.h"

#define INTERRUPT_IDT_NUM                   48

#define INTERRUPT_PIC_PORT1			        0x20
#define INTERRUPT_PIC_PORT2			        0xA0
#define INTERRUPT_PIC_EOI 			        0x20
#define INTERRUPT_PIC_VECTOR1_START	        32
#define INTERRUPT_PIC_VECTOR2_START	        40

#define INTERRUPT_GATE_TASK                 0x5
#define INTERRUPT_GATE_INTERRUPT            0x6
#define INTERRUPT_GATE_TRAP                 0x7

#define INTERRUPT_GATE_32BITS               0x8
#define INTERRUPT_GATE_16BITS               0x0

typedef struct interrupt_idtr{
	INT16U limit;				            /* IDT table size */
	INT32U base;				            /* physical memory address of IDT start */
}__attribute__((packed)) INTERRUPT_IDTR;

typedef struct interrupt_idt_desc
{
	INT16U offset1;				            /* memory offset bit 0~15 */
	INT16U selector;			            /* segment selector */
	INT8U zero;					            /* should be fill up with 0 */
	INT8U type:5;                           /* size of gate and gate type */
	INT8U dpl:2;                            /* descriptor privilege level */
	INT8U present:1;			            /* segment Present flag */
	INT16U offset2;				            /* memory offset bit 16~31 */
}__attribute__((packed)) INTERRUPT_IDT_DESC;

typedef struct interrupt_isr_desc
{
	void (*isr)();
	void (*handler)();
	INT8U type;
	INT8S *name;
}INTERRUPT_ISR_DESC;

INT8S interrupt_set_idt_desc(INT32U idt_sel, INT32U isr_addr, INT8U dpl, INT8U gate_type, void (*handler)());
void interrupt_pic_remap();
void interrupt_enable();
void interrupt_disable();
INT8S interrupt_set_isr_handler(INT8U idt_sel, void *p_handler);
void interrupt_load_idt(INTERRUPT_IDTR *p_system_idtr);
void interrupt_init();

#endif
