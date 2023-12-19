
BITS 16
org 0x7c00

jmp start

%include "gdt.asm"
%include "switch32.asm"

start:
	xor	ax, ax				; null segments
	mov	ds, ax
	mov	es, ax
	mov	ax, 0x9000			; stack begins at 0x9000-0xffff
	mov	ss, ax
	mov	sp, 0xFFFF
	
;--------------------------------------

     ; Install GDT
    cli
	lgdt [GDT_descriptor]
	
	; Switch 32 Bit
	call SWITCH_32
	
	cli                     ; off interrupt
	jmp CODE_SEG:Entry32Bit ; CS will be Auto-Updated to CODE_SEG

	jmp $

%include "print32.asm"

[BITS 32]

Entry32Bit:
	; Reload stack and data segment registers with GDT entry
	mov ax, DATA_SEG
	mov ds, ax
	mov ss, ax
	mov sp, 0xffff
	
    call Clear32

	mov si, Bit32Str
	call Print32
	
	; Halting the system
	cli
	hlt

Bit32Str db "Starting 32Bit B2-OS !", 0	

	times 510-($-$$) db 0
	dw 0xaa55