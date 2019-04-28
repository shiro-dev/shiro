[BITS 32]

; CONSTANTS
ARCHITECTURE 	equ 0; Protected mode i386
MAGIC 			equ 0xE85250D6; Magic Number for Multiboot 2 (GRUB2)
CHECKSUM_LEN 	equ grub.end - grub.start; Size of the checksum
CHECKSUM 		equ 0x100000000 - (0xe85250d6 + 0 + (grub.end - grub.start)); Checksum
FLAG_TYPE_1 	equ 0; Flag
FLAG_TYPE_2 	equ 0; Flag
FLAG_SIZE 		equ 8; Flag

; GLOBAL FUNCTIONS
global shiro.start:function (shiro.end - shiro.start); Make shiro.start visible

; EXTERNAL FUNCTIONS
extern shiro_main; Load shiro_main from c++

; GRUB
section .grub
grub:
.start:
    ; Set Magic Number
    dd MAGIC

	; Set Architecture
    dd ARCHITECTURE

	; Set Checksum
    dd CHECKSUM_LEN
    dd CHECKSUM

    ; Set Flags
    dw FLAG_TYPE_1
    dw FLAG_TYPE_2
    dd FLAG_SIZE
.end:

; BSS
section .bss
align 16
stack:
.bottom:
	resb 16384
.top:

; Shiro
section .shiro
shiro:
.start:
	mov esp, stack.top
	jmp shiro_main
.end: 
	cli
	hlt