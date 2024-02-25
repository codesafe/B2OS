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

	; protected mode로 가기전에 memory map 로딩
    mov bx, 0x0000	; 메모리맵 segment
    mov es, bx
    mov bx, 0x5C00	; 메모리맵 offset
    mov di, bx
	call load_memory_map	; es : di

	call enable_A20line

	; change to protected mode
    ; Install GDT
    cli
	lgdt [GDT_descriptor]
	sti

	; Switch 32 Bit
	call switch_32

	;cli                     ; off interrupt
	jmp CODE_SEG:Entry32Bit ; CS will be Auto-Updated to CODE_SEG
	jmp $


switch_32:					; clear interrupts
	mov	eax, cr0			; set bit 0 in cr0--enter pmode
	or	eax, 1
	mov	cr0, eax
	ret


;=================================================================================
; https://wiki.osdev.org/Detecting_Memory_(x86)#BIOS_Function:_INT_0x15.2C_EAX_.3D_0xE820
; Input:
;   es - buffer segment
;   di - buffer offset
load_memory_map:
    xor ebx, ebx

    ; Push initial buffer address
    push di
    ; Push initial entries count
    mov eax, 0
    push eax

load_memory_map_loop:
    ; Increment pointer in buffer
    add di, 24

    ; Increment entries count
    pop eax
    inc eax
    push eax

    ; Set magic number
    mov edx, 0x534d4150

    ; Set number of bytes to read
    mov ecx, 24

    ; Read memory map
    mov eax, 0xe820
    int 0x15
    
    ; Disable interrupts (can be enabled by int 0x15)
    cli

    ; Exit if ebx is equal to zero (reading has ended)
    cmp ebx, 0
    jne load_memory_map_loop

    ; Store entries count at the begin of the buffer
    pop eax
    pop di
    mov [di], eax
    
    ret

;=================================================================================

enable_A20line:
    push bp
    mov bp, sp
    mov ax, 0x2401
    int 0x15
    leave
    ret


%include "Asm/disk_read.asm"
%include "Asm/gdt.asm"

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