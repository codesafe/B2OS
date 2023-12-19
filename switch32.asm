BITS 16


SWITCH_32:					; clear interrupts
	mov	eax, cr0			; set bit 0 in cr0--enter pmode
	or	eax, 1
	mov	cr0, eax
	ret
