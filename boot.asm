[org 0x7c00]

mov ah, 0x0e

mov bp, 0x9000 ; set the stack
mov sp, bp

mov bx, MSG_REAL_MODE
call print ; This will be written after the BIOS messages

call switch_to_pm
jmp $ ; this will actually never be executed


;%include "../05-bootsector-functions-strings/boot_sect_print.asm"
%include "gdt.asm"
%include "print32.asm"
%include "switch32.asm"


the_secret:
    ; ASCII code 0x58 ('X') is stored just before the zero-padding.
    ; On this code that is at byte 0x2d (check it out using 'xxd file.bin')
    db "X"



; "oooooooooo.    .oooo.             .oooooo.    .oooooo..o", 0x0a,0x0d,
; "`888'   `Y8b .dP""Y88b           d8P'  `Y8b  d8P'    `Y8"
; " 888     888       ]8P'         888      888 Y88bo."
; " 888oooo888'     .d8P'          888      888  `"Y8888o."
; " 888    `88b   .dP'     8888888 888      888      `"Y88b" 
; " 888    .88P .oP     .o         `88b    d88' oo     .d8P"
; "o888bood8P'  8888888888          `Y8bood8P'  8""88888P'"
                                                         
                                                         
                                                         


; zero padding and magic bios number
times 510-($-$$) db 0
dw 0xaa55