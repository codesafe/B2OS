; cannot find entry symbol _start; defaulting to 0000000000001000 ( 경고 방지 )
global _start;
[bits 32]

_start:
    [extern kmain]
    call kmain
    jmp $