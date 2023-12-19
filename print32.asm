[bits 32]


VIDEO_MEMORY    equ 0xb8000
WHITE_ON_BLACK  equ 0x0f

;############################## Print32

Print32:
	mov edi, VIDEO_MEMORY
    mov ah, WHITE_ON_BLACK
    
.repeat:
	lodsb
	or al, al
	jz .done
	mov byte [edi], al
	inc edi
	mov byte [edi], 0x1B
	inc edi
	call .repeat
.done:
	ret
	    
;############################## Clear

Clear32 :
	mov edi, VIDEO_MEMORY
    mov ah, WHITE_ON_BLACK
    mov bx, 0xC80 ; 80x40

.c_repeat:
	lodsb
	or bx, bx
	jz .c_done

	mov byte [edi], ' ';al
	inc edi
	mov byte [edi], WHITE_ON_BLACK
	inc edi
    dec bx
	call .c_repeat
.c_done:
	ret