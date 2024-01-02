
[BITS 16]
[org 0x7c00]

KERNEL_LOCATION	equ	0x1000

jmp start

%include "Asm/gdt.asm"
%include "Asm/switch32.asm"

start:
    ; 나중을 위하여 boot drive를 기억 ( 0 : floppy , 1 : floppy2, 0x80 : hdd, 0x81 : hdd2 )
    ; 이것은 Bios에서 정해지는 값
	mov [BOOT_DISK], dl 

	xor ax, ax	; 0 reset
	mov es, ax
	mov ds, ax
	mov bp, 0x9000	; stack은 여기부터
	mov sp, bp

    ;[es:bx] 메모리 위치에 disk 읽어 로딩
	mov bx, KERNEL_LOCATION
    ; sector 몇개 읽을것인가? ( 작으면 리부팅됨 )
	mov dh, 32;31 ; Our future kernel will be larger, make this big

    ;pusha
    ;push dx
	mov al, dh      ; 읽을 sector 갯수    
	mov ah, 0x02    ; read
    mov cl, 0x02    ; sector 번호
	mov ch, 0x00    ; cylinder 번호
	mov dl, [BOOT_DISK]
	mov dh, 0x00    ; head 번호
	int 0x13
    ;pop dx
    ;popa

	; clear screen										
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

;Bit32Str db "Starting 32Bit B2-OS !", 0	

	times 510-($-$$) db 0
	dw 0xaa55