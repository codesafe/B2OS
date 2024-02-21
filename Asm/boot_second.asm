[BITS 16]
[org 0x7000]

	; load kernel
	mov bx, KERNELFILENAME
	call search_file

	; load kernel.bin to 0x0F000
	mov cx, KERNEL_LOCATION
	call load_file

	pusha
	mov si, load_kernel_str
	call print_str
	popa

;=================================================================================

	pusha
	mov si, switch32_str
	call print_str
	popa

	call enable_A20line

	; change to protected mode
    ; Install GDT
    cli
	lgdt [GDT_descriptor]
	
	; Switch 32 Bit
	call SWITCH_32

	cli                     ; off interrupt
	jmp CODE_SEG:Entry32Bit ; CS will be Auto-Updated to CODE_SEG
	jmp $

enable_A20line:
    push bp
    mov bp, sp
    mov ax, 0x2401
    int 0x15
    leave
    ret


%include "Asm/disk_read.asm"
%include "Asm/gdt.asm"
%include "Asm/switch32.asm"

;%include "Asm/memory.asm"

FAT12_LOCATION			equ	0x7E00
KERNEL_LOCATION			equ 0xF000
FAT12_SECTOR_COUNT		equ 32
BOOT_SECTOR_COUNT		equ 1
FAT_END_OF_CHAIN		equ 0x0FF0

SectorsPerTrack		dw 18
Head				dw 2
BytesPerSector		dw 512
FileAllocationTable	db 2
SectorsPerFAT		dw 9

; kernel.bin
KERNELFILENAME			db 'KERNEL  BIN'    ; 11 chars
BOOT_DISK		db 0 	; Boot device number
_cylinder   	db 0x00
_head       	db 0x00
_sector     	db 0x00

disk_read_ok		db 'READ OK', 0x0a, 0x0d, 0
disk_read_error		db 'READ ERR',0x0a, 0x0d, 0

;=================================================================================

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

	; start kernel ( kernel은 32bit 이므로...)
	jmp KERNEL_LOCATION
	jmp $
	; Halting the system
	;cli
	;hlt


load_kernel_str		db 'Load Kernel', 0x0a, 0x0d, 0
switch32_str		db 'Switch to 32Bit mode', 0x0a, 0x0d, 0