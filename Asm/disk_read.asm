
[BITS 16]

read_disk:

	mov ah, 0x02    ; read
	mov dl, [BOOT_DISK]
	; [es:bx] 0x1000:0x0000 => 0x10000 물리주소에 로딩
	int 0x13

	jc read_error

	cmp ah, 0
	jne read_error

	xor ah, ah
	mov bx, ax
	call print_dec

	mov bx, DISK_SUCC
	call print_string
	ret

read_error:
	mov bx, DISK_ERROR
	jmp disk_error

disk_param_error:
	mov bx, DISK_PARAM_ERROR
	jmp disk_error


disk_error:
	push ax
	call print_string
	pop ax
	mov bh, ah
	call print_dec
	jmp $

; 10진수 출력 10000단위 ~ 1단위
print_dec:
	mov dx, 0x00
	mov ax, bx
	mov bx, 10000
	div bx

	mov ah, 0x00
	call print_digit
	mov bx, dx
	
	mov dx, 0x00
	mov ax, bx
	mov bx, 1000
	div bx
	
	mov ah, 0x00
	call print_digit
	mov bx, dx
	
	mov dx, 0x00
	mov ax, bx
	mov bx, 100
	div bx

	mov ah, 0x00
	call print_digit
	mov bx, dx

	mov dx, 0x00
	mov ax, bx
	mov bx, 10
	div bx
	mov ah, 0x00
	call print_digit
	
	mov ax, dx
	mov ah, 0x00
	call print_digit
	ret
	
print_digit:
	mov ah, 0x0E
	add al, 48
	int 0x10
	ret

print_string:
	mov ah, 0x0E
	.loop:
		mov al, [bx]
		cmp al, 0x00
		je .end
		int 0x10
		inc bx
		jmp .loop
	.end:
		ret

; Input: ax : logical sector
; Output: cl - sector / ch - track / dh - head
LBAtoCHS:
	push bx
	push ax
	mov bx, ax			; Save logical sector
	mov dx, 0			; First the sector
	div word [SectorsPerTrack]
	add dl, 01h			; Physical sectors start at 1
	mov cl, dl			; Sectors belong in CL for int 13h
	mov ax, bx
	mov dx, 0			; Now calculate the head
	div word [SectorsPerTrack]
	mov dx, 0
	div word [Heads]
	mov dh, dl			; Head/side
	mov ch, al			; Track
	pop ax
	pop bx
	mov dl, byte [BOOT_DISK]		; Set correct device
	ret

BOOT_DISK:
	db 0x00

DISK_ERROR:
	db "disk_read_error", 0x0A, 0x0D, 0x00

DISK_PARAM_ERROR:
	db "disk_param_err", 0x0A, 0x0D, 0x00

DISK_SUCC:
	db "disk_read_ok", 0x0A, 0x0D, 0x00

SectorsPerTrack		dw 18		; 24th and 25th byte total= 2 bytes
Heads				dw 2		; 26th and 27th byte total= 2 bytes