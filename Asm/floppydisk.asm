
[BITS 16]

BOOT_DISK                 db 0x00
PhysicalSectorsPerTrack         dw 18
Heads                           dw 2

; Input: nothing
; Output:
;   - ah - disk operation status (http://stanislavs.org/helppc/int_13-1.html)
;   - cf - 1 if successful, 0 if error
ResetFloppy:
    ; Function name: Reset Disk System
    mov ah, 0
    int 0x13
    ret

;------------------------------------------------

lbatochs:			; Calculate head, track and sector settings for int 13h			
; IN: logical sector in AX, OUT: correct registers for int 13h
	push bx
	push ax
	mov bx, ax			; Save logical sector
	mov dx, 0			; First the sector
	div word [PhysicalSectorsPerTrack]
	add dl, 01h			; Physical sectors start at 1
	mov cl, dl			; Sectors belong in CL for int 13h
	mov ax, bx
	mov dx, 0			; Now calculate the head
	div word [PhysicalSectorsPerTrack]
	mov dx, 0
	div word [Heads]
	mov dh, dl			; Head/side
	mov ch, al			; Track
	pop ax
	pop bx
	;mov dl, byte [BOOT_DISK]		; Set correct device
	ret

; Input:
;   - bx - original sector number
; Output:
;   - cl - calculated sector number
;   - ch - calculated track number
;   - dh - calculated head number
GetCHS:
    push ebp
    mov  ebp, esp

    ; [ebp - 2] = Temp = LBA / (Sectors per Track)
    ; [ebp - 4] = Sector = (LBA % (Sectors per Track)) + 1
    mov ax, bx
    xor dx, dx
    mov cx, [PhysicalSectorsPerTrack]
    div cx
    push ax
    
    ;inc dx
    push dx

    ; [ebp - 6] = Head = Temp % (Number of Heads)
    ; [ebp - 8] = Track = Temp / (Number of Heads)
    mov ax, [ebp - 2]
    xor dx, dx
    mov bx, [Heads]
    div bx

    push dx
    push ax

    ; Sector number
    mov al, [ebp - 4]

    ; Track number
    mov cx, [ebp - 8]
    xchg ch, cl
    ror cl, 1
    ror cl, 1
    or cl, al
    inc cx

    ; Head number
    mov dh, [ebp - 6]

    mov esp, ebp
    pop ebp

    ret

;------------------------------------------------

; Input:
;   - al - sectors count
;   - cl - sector number
;   - es:bx - segment:offset
; Output:
;   - ah - status (http://stanislavs.org/helppc/int_13-1.html)
;   - al - number of sectors read
;   - cf - 1 if successful, 0 if error
LoadFloppyData:
    push ebp
    mov ebp, esp
    
    movzx si, al    ; al을 si로 복사하는데 si의 상위 Bit는 0으로 채워짐
    ; 결국 al 만큼 반복하겠다

    push ax     ; ax = [ebp - 2]
    push bx     ; bx = [ebp - 4]
    push cx     ; cx = [ebp - 6]

loadfloppy_loop:
    ; push ax
    ; push bx
    ; mov bx, si
    ; call print_dec
    ; pop bx
    ; pop ax

    mov bx, cx
    call GetCHS
    ;movzx ax, cl
    ;call lbatochs

    mov ax, [ebp - 2]
    mov bx, [ebp - 4]

    pusha
    movzx bx, cl
    call print_dec  ; sector
    movzx bx, ch
    call print_dec ; track
    ;movzx bx, dh
    ;call print_dec    
    ;mov ah, 0x0E
	mov al, '|'
	int 0x10
    popa

    ; Drive number
    mov dl, [BOOT_DISK]
    mov ax, 0x0201  ; 읽기 / 1 섹터 ah:x02 al:x01
    int 0x13
    dec si
    jz loadfloppy_exit

    add bx, 0x200   ; 512 byte
    mov [ebp - 4], bx
    inc word [ebp - 6]
    mov cx, [ebp - 6]
    jmp loadfloppy_loop

loadfloppy_exit:
    mov esp, ebp
    pop ebp
    ret

load_error:
    mov bx, DISK_ERROR
    call print_string
    jmp $

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


; 10진수 출력 10000단위 ~ 1단위
print_dec:
	; mov dx, 0x00
	; mov ax, bx
	; mov bx, 10000
	; div bx

	; mov ah, 0x00
	; call print_digit
	; mov bx, dx
	
	; mov dx, 0x00
	; mov ax, bx
	; mov bx, 1000
	; div bx

	; mov ah, 0x00
	; call print_digit
	; mov bx, dx

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

	;mov al, 0x0A
	;int 0x10    
	;mov al, 0x0D
	;int 0x10    
	mov al, ' '
	int 0x10        
	ret
	

print_digit:
	mov ah, 0x0E
	add al, 48
	int 0x10
	ret

DISK_DONE:
	db "DONE", 0x0A, 0x0D, 0x00        

DISK_ERROR:
	db "ERR", 0x0A, 0x0D, 0x00            