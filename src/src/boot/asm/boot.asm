[BITS 32]

; GLOBAL FUNCTIONS
global shiro.start:function (shiro.end - shiro.start); Make shiro.start visible
global grub
global outb

; EXTERNAL FUNCTIONS
extern shiro_main; Load shiro_main from c++

; CONSTANTS
ARCHITECTURE 	equ 0; Protected mode i386
MAGIC 			equ 0xE85250D6; Magic Number for Multiboot 2 (GRUB2)
CHECKSUM_LEN 	equ grub.end - grub.start; Size of the checksum
CHECKSUM 		equ 0x100000000 - (0xe85250d6 + 0 + (grub.end - grub.start)); Checksum
VIDEO_USE_TEXT  equ 0; 0 for graphics mode, 1 for text mode
VIDEO_WIDTH     equ 1280; width (pixels for graphics and characters for text mode)
VIDEO_HEIGHT    equ 768; height
VIDEO_BPP       equ 32; bpp (ignored for text mode)

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

    ; Align Tags
    dw 6
    dw 0
    dd 8

    ; End Tags
    dw 0
    dw 0
    dd 8
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
    push ebx
	call shiro_main
.end: 
	cli
	hlt

; outb - send a byte to an I/O port
; stack: [esp + 8] the data byte
;        [esp + 4] the I/O port
;        [esp    ] return address
outb:
    mov al, [esp + 8]    ; move the data to be sent into the al register
    mov dx, [esp + 4]    ; move the address of the I/O port into the dx register
    out dx, al           ; send the data to the I/O port
    ret                  ; return to the calling function

; Include a Real Mode Switcher
%include "real-mode-switcher.asm"