
[BITS 16]
[org 0x7c00]

KERNEL_LOCATION	equ	0x1000

jmp start

%include "Asm/gdt.asm"
%include "Asm/switch32.asm"

start:
	mov [BOOT_DISK], dl             

	xor ax, ax                          
	mov es, ax
	mov ds, ax
	mov bp, 0x8000
	mov sp, bp

	mov bx, KERNEL_LOCATION
	mov dh, 2

	mov ah, 0x02
	mov al, dh 
	mov ch, 0x00
	mov dh, 0x00
	mov cl, 0x02
	mov dl, [BOOT_DISK]
	int 0x13                ; no error management, do your homework!

										
	mov ah, 0x0
	mov al, 0x3
	int 0x10                ; text mode
	
;--------------------------------------

     ; Install GDT
    cli
	lgdt [GDT_descriptor]
	
	; Switch 32 Bit
	call SWITCH_32
	
	cli                     ; off interrupt
	jmp CODE_SEG:Entry32Bit ; CS will be Auto-Updated to CODE_SEG

	jmp $

	BOOT_DISK: db 0

%include "Asm/print32.asm"

[BITS 32]

Entry32Bit:
	; Reload stack and data segment registers with GDT entry
	mov ax, DATA_SEG
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	mov ebp, 0x90000		; 32 bit stack base pointer
	mov esp, ebp
	
    ;call Clear32

	;mov si, Bit32Str
	;call Print32

	jmp KERNEL_LOCATION

	; Halting the system
	cli
	hlt



Bit32Str db "Starting 32Bit B2-OS !", 0	

	times 510-($-$$) db 0
	dw 0xaa55