;******************************************************************************
; boot.asm (a simple bootloader that require by GRUB)
; 20101218 shiuan
;******************************************************************************
		GLOBAL boot, _p_boot_info           	; making entry point visible to linker
		EXTERN kernel_main            			; kmain is defined elsewhere

		; setting up the Multiboot header - see GRUB docs for details
		MODULEALIGN EQU 1 << 0                 	; align loaded modules on page boundaries
		MEMINFO     EQU 1 << 1                 	; provide memory map
		FLAGS       EQU MODULEALIGN | MEMINFO  	; this is the Multiboot 'flag' field
		MAGIC       EQU 0x1BADB002           	; 'magic number' lets bootloader find the header
		CHECKSUM    EQU -(MAGIC + FLAGS)       	; checksum required
		STACKSIZE 	EQU 0x4000                  ; stack size

		SECTION .text
		ALIGN 4

MultiBootHeader:
		DD MAGIC
		DD FLAGS
		DD CHECKSUM

boot:
		mov esp, stack+STACKSIZE           		; set up the stack
		cmp eax, 0x2BADB002
		jne hang
		;push eax                               ; pass Multiboot magic number
		;push ebx                           	; pass Multiboot info structure
		mov [_p_boot_info], ebx
		call  kernel_main                       ; call kernel proper
hang:	cli
		hlt                                		; halt machine should kernel return
		jmp   hang

		SECTION .data
		_p_boot_info DD 0

		SECTION .bss
		ALIGN 4
stack:
		RESB STACKSIZE                     		; reserve 16k stack on a doubleword boundary
