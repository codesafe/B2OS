
[BITS 16]

reset_disk:
    xor ax,ax
    int 0x13
    ret

;===============================================================
; http://www.osdever.net/tutorials/view/lba-to-chs
; ax : lba
; div : ax : 몫(Quotient), dx : 나머지(Remainder)
lbatochs:
	xor dx,dx
	; first calcuation is sector: sector = (lba % SectorsPerTrack + 1)
	div word [SectorsPerTrack]		; SectorsPerTrack : 18개
	inc dl							; Physical sectors start at 1
	mov byte [_sector],dl			; 섹터

	xor dx,dx
	; head = (lba / SectorsPerTrack) % NumHeads => (나머지)
	div word [Head]
	mov byte [_head],dl

	; Cylinder(track) = (lba / SectorsPerTrack)/NumHeads => (몫)
	mov byte [_cylinder],al
	ret

;===============================================================

; lba : ax
read_disk:
	mov di,0x5			; 오류시 재시도 횟수

read_loop:
	; we need to preserve these between reads
	push ax
	push bx
	push cx

	call lbatochs
	mov ax,0x0201				; ah = bios function 2, al = read 1 sector
	mov ch,byte [_cylinder]
	mov cl,byte [_sector]
	mov dh,byte [_head]
	mov dl,byte [BOOT_DISK]
	int 0x13
	jnc read_ok

	; try to reset the disk for retry
	call reset_disk
	dec di

	; restore for the next attempt
	pop cx
	pop bx
	pop ax
	jnz read_loop
	stc
	jmp read_done

read_ok:

%ifdef READ_PROGRESS
 	mov si,read_progress
 	call print_str
%endif

	; restore to move on to the next sector
	pop cx
	pop bx
	pop ax

	add bx,word [BytesPerSector]	; es:bx -> 1 sector (512)
	inc ax
	loop read_disk
	clc

read_done:
	jc read_error
	ret


read_error:
	mov si,disk_read_error
	call print_str
	cli
	jmp $
	

;----------------------------------------
; data section for floppy operations
;----------------------------------------
_cylinder   	db 0x00
_head       	db 0x00
_sector     	db 0x00


%include "print16.asm"

%ifdef READ_PROGRESS
read_progress		db '.', 0
%endif

disk_read_error		db 'DISK ERR',0


; read_disk:

; 	mov ah, 0x02    ; read
; 	mov dl, [BOOT_DISK]
; 	; [es:bx] 0x1000:0x0000 => 0x10000 물리주소에 로딩
; 	int 0x13

; 	jc read_error

; 	cmp ah, 0
; 	jne read_error

; 	xor ah, ah
; 	mov bx, ax
; 	call print_dec

; 	mov bx, DISK_SUCC
; 	call print_string
; 	ret

; read_error:
; 	mov bx, DISK_ERROR
; 	jmp disk_error

; disk_param_error:
; 	mov bx, DISK_PARAM_ERROR
; 	jmp disk_error


; disk_error:
; 	push ax
; 	call print_string
; 	pop ax
; 	mov bh, ah
; 	call print_dec
; 	jmp $

; ; 10진수 출력 10000단위 ~ 1단위
; print_dec:
; 	mov dx, 0x00
; 	mov ax, bx
; 	mov bx, 10000
; 	div bx

; 	mov ah, 0x00
; 	call print_digit
; 	mov bx, dx
	
; 	mov dx, 0x00
; 	mov ax, bx
; 	mov bx, 1000
; 	div bx
	
; 	mov ah, 0x00
; 	call print_digit
; 	mov bx, dx
	
; 	mov dx, 0x00
; 	mov ax, bx
; 	mov bx, 100
; 	div bx

; 	mov ah, 0x00
; 	call print_digit
; 	mov bx, dx

; 	mov dx, 0x00
; 	mov ax, bx
; 	mov bx, 10
; 	div bx
; 	mov ah, 0x00
; 	call print_digit
	
; 	mov ax, dx
; 	mov ah, 0x00
; 	call print_digit
; 	ret
	
; print_digit:
; 	mov ah, 0x0E
; 	add al, 48
; 	int 0x10
; 	ret

; print_string:
; 	mov ah, 0x0E
; 	.loop:
; 		mov al, [bx]
; 		cmp al, 0x00
; 		je .end
; 		int 0x10
; 		inc bx
; 		jmp .loop
; 	.end:
; 		ret



; ;BOOT_DISK:
; ;	db 0x00

; DISK_ERROR:
; 	db "disk_read_error", 0x0A, 0x0D, 0x00

; DISK_PARAM_ERROR:
; 	db "disk_param_err", 0x0A, 0x0D, 0x00

; DISK_SUCC:
; 	db "disk_read_ok", 0x0A, 0x0D, 0x00

; SectorsPerTrack		dw 18		; 24th and 25th byte total= 2 bytes
; Heads				dw 2		; 26th and 27th byte total= 2 bytes