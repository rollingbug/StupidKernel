;
;******************************************************************************
; My Stupid Kernel
;
;
; File Name:   isr.asm
; File Desc:   isr table mapping
; Author:      Y.S.Kuo
; Date:        20130119
;******************************************************************************
;
		GLOBAL
		EXTERN isr_dispatcher

		%MACRO NO_ERR_CODE_ISR 2
		GLOBAL %1
		%1:
		push dword 0
		push dword %2
		call isr_dispatcher
		add esp, 8
		iret
		%ENDMACRO

		%MACRO ERR_CODE_ISR 2
		GLOBAL %1
		%1:
		push dword %2
		call isr_dispatcher
		add esp, 8
		iret
		%ENDMACRO

		SECTION .text
		ALIGN 4
		;cpu exceptions
		NO_ERR_CODE_ISR interrupt_cpu_exection0, 	0
		NO_ERR_CODE_ISR interrupt_cpu_exection1, 	1
		NO_ERR_CODE_ISR interrupt_cpu_exection2, 	2
		NO_ERR_CODE_ISR interrupt_cpu_exection3, 	3
		NO_ERR_CODE_ISR interrupt_cpu_exection4, 	4
		NO_ERR_CODE_ISR interrupt_cpu_exection5, 	5
		NO_ERR_CODE_ISR interrupt_cpu_exection6, 	6
		NO_ERR_CODE_ISR interrupt_cpu_exection7, 	7
		ERR_CODE_ISR 	interrupt_cpu_exection8, 	8
		NO_ERR_CODE_ISR interrupt_cpu_exection9, 	9
		ERR_CODE_ISR 	interrupt_cpu_exection10, 	10
		ERR_CODE_ISR 	interrupt_cpu_exection11, 	11
		ERR_CODE_ISR 	interrupt_cpu_exection12, 	12
		ERR_CODE_ISR 	interrupt_cpu_exection13, 	13
		ERR_CODE_ISR 	interrupt_cpu_exection14, 	14
		NO_ERR_CODE_ISR interrupt_cpu_exection15, 	15
		NO_ERR_CODE_ISR interrupt_cpu_exection16, 	16
		ERR_CODE_ISR 	interrupt_cpu_exection17, 	17
		NO_ERR_CODE_ISR interrupt_cpu_exection18, 	18
		NO_ERR_CODE_ISR interrupt_cpu_exection19, 	19
		NO_ERR_CODE_ISR interrupt_cpu_exection20, 	20
		NO_ERR_CODE_ISR interrupt_cpu_exection21, 	21
		NO_ERR_CODE_ISR interrupt_cpu_exection22, 	22
		NO_ERR_CODE_ISR interrupt_cpu_exection23, 	23
		NO_ERR_CODE_ISR interrupt_cpu_exection24, 	24
		NO_ERR_CODE_ISR interrupt_cpu_exection25, 	25
		NO_ERR_CODE_ISR interrupt_cpu_exection26, 	26
		NO_ERR_CODE_ISR interrupt_cpu_exection27, 	27
		NO_ERR_CODE_ISR interrupt_cpu_exection28, 	28
		NO_ERR_CODE_ISR interrupt_cpu_exection29, 	29
		NO_ERR_CODE_ISR interrupt_cpu_exection30, 	30
		NO_ERR_CODE_ISR interrupt_cpu_exection31, 	31

		;hardware interrupts
		NO_ERR_CODE_ISR interrupt_hw_interrupt0, 	32
		NO_ERR_CODE_ISR interrupt_hw_interrupt1, 	33
		NO_ERR_CODE_ISR interrupt_hw_interrupt2, 	34
		NO_ERR_CODE_ISR interrupt_hw_interrupt3, 	35
		NO_ERR_CODE_ISR interrupt_hw_interrupt4, 	36
		NO_ERR_CODE_ISR interrupt_hw_interrupt5, 	37
		NO_ERR_CODE_ISR interrupt_hw_interrupt6, 	38
		NO_ERR_CODE_ISR interrupt_hw_interrupt7, 	39
		NO_ERR_CODE_ISR interrupt_hw_interrupt8, 	40
		NO_ERR_CODE_ISR interrupt_hw_interrupt9, 	41
		NO_ERR_CODE_ISR interrupt_hw_interrupt10, 	42
		NO_ERR_CODE_ISR interrupt_hw_interrupt11, 	43
		NO_ERR_CODE_ISR interrupt_hw_interrupt12, 	44
		NO_ERR_CODE_ISR interrupt_hw_interrupt13, 	48
		NO_ERR_CODE_ISR interrupt_hw_interrupt14, 	49
		NO_ERR_CODE_ISR interrupt_hw_interrupt15, 	50
