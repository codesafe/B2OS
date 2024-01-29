
[BITS 16]

; 0이 나올때까지 print / 0a,0d 자동 포함
print_str:
	pusha
	mov ah,0x0e
	mov cx,0x01

print_loop:
	lodsb
	or al,al
	jz print_done

	int 0x10
	jmp print_loop

print_done:
	;mov al,0x0a
	;int 0x10
	;mov al,0x0d
	;int 0x10
	popa
	ret
