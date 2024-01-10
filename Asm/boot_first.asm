
[BITS 16]
[org 0x7c00]

BOOT_SECOND	equ	0x7e00

boot_first:
    ; 나중을 위하여 boot drive를 기억 ( 0 : floppy , 1 : floppy2, 0x80 : hdd, 0x81 : hdd2 )
    ; 이것은 Bios에서 정해지는 값
	mov [BOOT_DISK], dl 

	cli	; stop interrupt
	xor ax, ax	; 0 reset
	mov ds, ax
	mov es, ax	; [es:bx]
	mov ss, ax
	mov sp, 0x7c00
	mov bp, 0x9000	; stack은 여기부터
	sti	; enable interrupt

	; clear screen										
	mov ah, 0x0
	mov al, 0x3
	int 0x10                ; text mode

	mov bx, BOOT_SECOND ;[es:bx] 메모리 위치에 disk 읽어 로딩

	mov al, 0x02    ; 읽을 sector 갯수 ; sector 몇개 읽을것인가? ( 작으면 리부팅됨 )
	; floppy의 1sector는 512 byte	
    mov cl, 0x02    ; sector 번호 (boot loader가 첫번째 512byte, 2번째 sector)
	mov ch, 0x00    ; cylinder 번호
	mov dh, 0x00    ; head 번호
	call read_disk

	jmp BOOT_SECOND

%include "Asm/disk_read.asm"

times 510-($-$$) db 0	; Boot loader를 512에 맞춤 (마지막 aa55를 빼고 코드부분이외 나머지를 0으로 총510바이트 + aa55 = 512)
dw 0xaa55