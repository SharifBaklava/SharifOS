section .multiboot
header_start:
	; magic number
	dd 0xe85250d6 ; multiboot2
	; architecture
	dd 0 ; protected mode i386
	; header length
	dd header_end - header_start
	; checksum
	dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

	; end taga
	dw 0
	dw 0
	dd 8
header_end:

global start
extern kernel_main


section .bss
align 16
stack_bottom:
times 16384 db 0;	 16 KiB
stack_top:

section .text
bits 32
start:
	mov esp, stack_top

	; call check_long_mode
	call kernel_main
	; call setup_page_tables
	; call enable_paging
	cli
	hlt
