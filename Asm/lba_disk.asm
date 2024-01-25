
[BITS 16]

lba_read_disk:
	pusha

lba_disk_reset:
; DAP ( https://manggong.org/39 )
;| DAP 크기 | 1 Byte | 구조체 크기 총 16 (0x10) 으로 설정
;| 예약  	| 1 Byte | 사용 안한다고 한다. 그냥 0
;| 읽을 섹터 수 | 2 Byte | 읽어야 할 섹터 수
;| 저장할메모리 | 4 Byte | 세그먼트:오프셋 으로 읽은 데이터를 저장 할 메모리 위치
;| 읽을 위치 	| 8 Byte | 읽을 시작 섹터 번호(0 base index)
	xor ax, ax
	mov dl, [BOOT_DISK]
	int 0x13
	jc lba_disk_reset

	; DAP 시작
	mov	[dap.count], ax			; count
	mov	[dap.offset], bx		; destination low part
	mov	[dap.segment], cx		; destination high part
	mov	edx, [current_lba]
	mov	[dap.lba], edx			; source
	add	[current_lba], eax		; update current_lba	

	mov ax, 0x500	; DAP의 seg:off
	mov si, ax
	mov ax, 0
	mov ds, ax

	mov ah, 0x42	; lba read command
	mov si, dap
	mov dl, [BOOT_DISK]
	int 0x13
	jc lba_disk_error

	mov bx, DISK_OK
	call print_string
	popa
	ret


lba_disk_error:
	mov bx, DISK_ERROR
	call print_string
	popa
	ret

print_string:
	mov ah, 0x0e
	.loop:
		mov al, [bx]
		cmp al, 0x00
		je .end
		int 0x10
		inc bx
		jmp .loop
	.end:
		ret


BOOT_DISK		equ 0x550

DISK_OK:
	db "OK", 0x0A, 0x0D, 0x00

DISK_ERROR:
	db "ERR", 0x0A, 0x0D, 0x00

SECOND_STEP :	
	db "2ND", 0x0A, 0x0D, 0x00

current_lba	dd	1

dap:
	db	0x10			; size of DAP
	db	0				; unused
.count:
	dw	0				; number of sectors
.offset:
	dw	0				; destination: offset
.segment:
	dw	0				; destination: segment
.lba:
	dd	0				; low bytes of LBA address
	dd	0				; high bytes of LBA address