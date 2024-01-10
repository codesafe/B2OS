
[BITS 16]
[org 0x7e00]

KERNEL_LOCATION	equ	0x8200

;boot_second:
	mov bx, KERNEL_LOCATION
	mov al, 63		; read 60 sectors
	mov ch, 0x00		; from cylinder 0
	mov cl, 0x04		; sector 4 boot_first가 2부터 2개 2,3 읽음 그러므로 4부터
	mov dh, 0x00		; from head 0
	call read_disk

;--------------------------------------

     ; Install GDT
    cli
	lgdt [GDT_descriptor]
	
	; Switch 32 Bit
	call SWITCH_32
	
	cli                     ; off interrupt
	jmp CODE_SEG:Entry32Bit ; CS will be Auto-Updated to CODE_SEG
	jmp $


%include "Asm/disk_read.asm"
%include "Asm/gdt.asm"
%include "Asm/switch32.asm"
%include "Asm/print32.asm"
;%include "Asm/memory.asm"

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

	;enable the A20 line
	in al, 0x92
	or al, 0x02
	out 0x92, al

	jmp KERNEL_LOCATION
	jmp $
	; Halting the system
	;cli
	;hlt

;Bit32Str db "Starting 32Bit B2-OS !", 0	
; 2섹터 == 1024 : 나머지를 0으로 1024 까지 채운다
times 1024-($-$$) db 0x00