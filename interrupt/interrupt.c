/*
 *****************************************************************************
 * My Stupid Kernel
 *
 *
 * File Name:   interrupt.c
 * File Desc:   function to initial and control system interrupt
 * Author:      Y.S.Kuo
 * Date:        20130119
 *****************************************************************************
 */

#include "interrupt.h"
#include "../kernel.h"
#include "../mm/mm.h"

extern void interrupt_cpu_exection0();
extern void interrupt_cpu_exection1();
extern void interrupt_cpu_exection2();
extern void interrupt_cpu_exection3();
extern void interrupt_cpu_exection4();
extern void interrupt_cpu_exection5();
extern void interrupt_cpu_exection6();
extern void interrupt_cpu_exection7();
extern void interrupt_cpu_exection8();
extern void interrupt_cpu_exection9();
extern void interrupt_cpu_exection10();
extern void interrupt_cpu_exection11();
extern void interrupt_cpu_exection12();
extern void interrupt_cpu_exection13();
extern void interrupt_cpu_exection14();
extern void interrupt_cpu_exection15();
extern void interrupt_cpu_exection16();
extern void interrupt_cpu_exection17();
extern void interrupt_cpu_exection18();
extern void interrupt_cpu_exection19();
extern void interrupt_cpu_exection20();
extern void interrupt_cpu_exection21();
extern void interrupt_cpu_exection22();
extern void interrupt_cpu_exection23();
extern void interrupt_cpu_exection24();
extern void interrupt_cpu_exection25();
extern void interrupt_cpu_exection26();
extern void interrupt_cpu_exection27();
extern void interrupt_cpu_exection28();
extern void interrupt_cpu_exection29();
extern void interrupt_cpu_exection30();
extern void interrupt_cpu_exection31();

extern void interrupt_hw_interrupt0();
extern void interrupt_hw_interrupt1();
extern void interrupt_hw_interrupt2();
extern void interrupt_hw_interrupt3();
extern void interrupt_hw_interrupt4();
extern void interrupt_hw_interrupt5();
extern void interrupt_hw_interrupt6();
extern void interrupt_hw_interrupt7();
extern void interrupt_hw_interrupt8();
extern void interrupt_hw_interrupt9();
extern void interrupt_hw_interrupt10();
extern void interrupt_hw_interrupt11();
extern void interrupt_hw_interrupt12();
extern void interrupt_hw_interrupt13();
extern void interrupt_hw_interrupt14();
extern void interrupt_hw_interrupt15();

static INTERRUPT_IDTR interrupt_idtr;
static INTERRUPT_IDT_DESC interrupt_idt_desc[INTERRUPT_IDT_NUM];
static INTERRUPT_ISR_DESC interrupt_isr_vector[INTERRUPT_IDT_NUM] = {	/* cpu exceptions */
									{interrupt_cpu_exection0, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Divide Error"},
									{interrupt_cpu_exection1, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Debug"},
									{interrupt_cpu_exection2, 	NULL, INTERRUPT_GATE_INTERRUPT,	"NMI interrupt"},
									{interrupt_cpu_exection3, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Breakpoint"},
									{interrupt_cpu_exection4, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Overflow"},
									{interrupt_cpu_exection5, 	NULL, INTERRUPT_GATE_INTERRUPT,	"BOUND Range Exceeded"},
									{interrupt_cpu_exection6, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Invalid Opcode"},
									{interrupt_cpu_exection7, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Device Not Available"},
									{interrupt_cpu_exection8, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Double Fault"},
									{interrupt_cpu_exection9, 	NULL, INTERRUPT_GATE_INTERRUPT,	"CoProcessor Segment Overrun"},
									{interrupt_cpu_exection10, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Invalid TSS"},
									{interrupt_cpu_exection11, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Segment Not Present"},
									{interrupt_cpu_exection12, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Stack Segment Fault"},
									{interrupt_cpu_exection13, 	NULL, INTERRUPT_GATE_INTERRUPT,	"General Protection"},
									{interrupt_cpu_exection14, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Page Fault"},
									{interrupt_cpu_exection15, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Reserved"},
									{interrupt_cpu_exection16, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Floating-Point Error"},
									{interrupt_cpu_exection17, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Alignment Check"},
									{interrupt_cpu_exection18, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Machine Check"},
									{interrupt_cpu_exection19, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Streaming SIMD Extensions"},
									{interrupt_cpu_exection20, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Reserved"},
									{interrupt_cpu_exection21, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Reserved"},
									{interrupt_cpu_exection22, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Reserved"},
									{interrupt_cpu_exection23, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Reserved"},
									{interrupt_cpu_exection24, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Reserved"},
									{interrupt_cpu_exection25, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Reserved"},
									{interrupt_cpu_exection26, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Reserved"},
									{interrupt_cpu_exection27, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Reserved"},
									{interrupt_cpu_exection28, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Reserved"},
									{interrupt_cpu_exection29, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Reserved"},
									{interrupt_cpu_exection30, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Reserved"},
									{interrupt_cpu_exection31, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Reserved"},

									/* hardware interrupts */
									{interrupt_hw_interrupt0, 	NULL, INTERRUPT_GATE_INTERRUPT,	"System Timer"},
									{interrupt_hw_interrupt1, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Keyboard"},
									{interrupt_hw_interrupt2, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Redirected IRQ1"},
									{interrupt_hw_interrupt3, 	NULL, INTERRUPT_GATE_INTERRUPT,	"COM2"},
									{interrupt_hw_interrupt4, 	NULL, INTERRUPT_GATE_INTERRUPT,	"COM1"},
									{interrupt_hw_interrupt5, 	NULL, INTERRUPT_GATE_INTERRUPT,	"LPT2"},
									{interrupt_hw_interrupt6, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Floppy Disk Controller"},
									{interrupt_hw_interrupt7, 	NULL, INTERRUPT_GATE_INTERRUPT,	"LPT1"},
									{interrupt_hw_interrupt8, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Real Time Clock"},
									{interrupt_hw_interrupt9, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Redirected IRQ2"},
									{interrupt_hw_interrupt10, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Reserved"},
									{interrupt_hw_interrupt11, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Reserved"},
									{interrupt_hw_interrupt12, 	NULL, INTERRUPT_GATE_INTERRUPT,	"PS/2 Mouse"},
									{interrupt_hw_interrupt13, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Math's Co-Processor"},
									{interrupt_hw_interrupt14, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Hard Disk Drive"},
									{interrupt_hw_interrupt15, 	NULL, INTERRUPT_GATE_INTERRUPT,	"Reserved"}
							   };

/* basic isr handler */
void isr_dispatcher(INT32U isr_num)
{
    if(interrupt_isr_vector[isr_num].handler != NULL)
        interrupt_isr_vector[isr_num].handler();
    else
        KERNEL_MSG(0, 0, "[%s]\n", interrupt_isr_vector[isr_num].name);

	/* send EOI command to 8259A */
	if(isr_num >= INTERRUPT_PIC_VECTOR1_START && isr_num < INTERRUPT_PIC_VECTOR1_START + 8)
		outb(INTERRUPT_PIC_PORT1, INTERRUPT_PIC_EOI);
	else if(isr_num >= INTERRUPT_PIC_VECTOR2_START && isr_num < INTERRUPT_PIC_VECTOR2_START + 8)
		outb(INTERRUPT_PIC_PORT2, INTERRUPT_PIC_EOI);
}

INT8S interrupt_set_idt_desc(INT32U idt_sel, INT32U isr_addr, INT8U dpl, INT8U gate_type, void (*handler)())
{
    if(idt_sel > INTERRUPT_IDT_NUM - 1)
        return -1;

    interrupt_idt_desc[idt_sel].offset1 = (isr_addr & 0x0000FFFF);
	interrupt_idt_desc[idt_sel].selector = KERNEL_CODE_SEL;
	interrupt_idt_desc[idt_sel].zero = 0;
	interrupt_idt_desc[idt_sel].present = 1;
	interrupt_idt_desc[idt_sel].dpl = dpl;
	interrupt_idt_desc[idt_sel].type = INTERRUPT_GATE_32BITS | gate_type;
	interrupt_idt_desc[idt_sel].offset2 = ((isr_addr & 0xFFFF0000) >> 16);

	interrupt_isr_vector[idt_sel].handler = handler;

    return 0;
}

INT8S interrupt_set_isr_handler(INT8U idt_sel, void *p_handler)
{
    if(idt_sel > INTERRUPT_IDT_NUM)
        return -1;
    if(p_handler == NULL)
        return -1;

    interrupt_isr_vector[idt_sel].handler = p_handler;

    return 0;
}

/* remap pic irq number */
void interrupt_pic_remap()
{
    outb(0x20, 0x11);	/* ICW1 */
    outb(0xA0, 0x11);
    outb(0x21, 0x20);	/* remap IRQ0~7 to 32~39 in interrupt vector */
    outb(0xA1, 0x28);	/* remap IRQ8~15 to 40~47 in interrupt vector */

    outb(0x21, 0x04);	/* ICW3 */
    outb(0xA1, 0x02);

    outb(0x21, 0x01);	/* ICW4 */
    outb(0xA1, 0x01);

    outb(0x21, 0x01);	/* enable all interrupt */
    outb(0xA1, 0x00);
}

void interrupt_enable()
{
	asm("sti");
}

void interrupt_disable()
{
	asm("cli");
}

void interrupt_load_idt(INTERRUPT_IDTR *p_system_idtr)
{
    asm("lidt (%0)"
        :
        : "r" (p_system_idtr)
    );
}

void interrupt_init()
{
	INT16U idt_count;

	interrupt_idtr.limit = sizeof(INTERRUPT_IDT_DESC) * INTERRUPT_IDT_NUM - 1;
	interrupt_idtr.base = (INT32U)interrupt_idt_desc;

	for(idt_count = 0; idt_count < INTERRUPT_IDT_NUM; idt_count++){
	    interrupt_set_idt_desc(idt_count, (INT32U)interrupt_isr_vector[idt_count].isr,
                       KERNEL_PRIV_KERNEL, interrupt_isr_vector[idt_count].type,
                       interrupt_isr_vector[idt_count].handler);
	}

    interrupt_disable();
	interrupt_pic_remap();
	interrupt_load_idt(&interrupt_idtr);
	interrupt_enable();
}
