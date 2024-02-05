;                                            MicrOS physic memory layout
;
; |------------|---------|------------|------------|----------------|---------|------------|------------|---------|---------|------------|------------|------------|------------|
; |   1 KiB    |  256 B  |   21 KiB   |   1 KiB    |     4 KiB      |  3 KiB  |   512 B    |   28 KiB   | 578 KiB |  1 KiB  |  383 KiB   |   15 MiB   |   1 MiB    |   4 MiB    |
; | Interrupts |   BDA   | Floppy DMA | Memory map | Page Directory |  Free   | Bootloader | FAT32 data |  Kernel |  EBDA   | Video, ROM |  Reserved  |   Stack    | Page Table |
; |            |         |            |            |                |         |            |            |         |         |            |            |            |            |
; |  0x00000   | 0x00400 |  0x00500   |  0x05C00   |    0x06000     | 0x07000 |  0x07C00   |  0x07E00   | 0x0F000 | 0x9FC00 |  0xA0000   | 0x00100000 | 0x01000000 | 0x01100000 |
; |  0x003FF   | 0x004FF |  0x05BFF   |  0x05FFF   |    0x06FFF     | 0x07BFF |  0x07DFF   |  0x0EFFF   | 0x9FBDD | 0x9FFFF |  0xFFFFF   | 0x00FFFFFF | 0x010FFFFF | 0x014FFFFF |
; |------------|---------|------------|------------|----------------|---------|------------|------------|---------|---------|------------|------------|------------|------------|
; {                                        Segment 1                                                       }{                            Segment 2 - n                          }

[BITS 16]
[ORG 0x7C00]

; Jump to main and align to 3 bytes, we don't want to execute FAT12 header
jmp Main

; Function for displaying debug text in real mode
nop

; FAT header
OEMName                         db 'MicrOS  '       ; 8 chars
BytesPerLogicalSector           dw 0x0200

; Bios parameter block
LogicalSectorsPerCluster        db 0x01
ReservedLogicalSectors          dw 0x0001
FileAllocationTables            db 0x02
MaxRootDirectoryEntries         dw 0x00E0
TotalLogicalSectors             dw 0x0B40
MediaDescriptor                 db 0xF0
LogicalSectorsPerFAT            dw 0x0009
PhysicalSectorsPerTrack         dw 0x0012
Heads                           dw 0x0002
HiddenSectors                   dd 0x00000000
TotalLogicalAndHiddenSectors    dd 0x00000000
PhysicalDriveNumber             db 0x00
INT13Scratchpad                 db 0x00
ExtendedBootSignature           db 0x29
VolumeID                        dd 0x12345678
PartitionVolumeLabel            db 'SYSTEM PART'    ; 11 chars
FileSystemType                  db 'FAT12   '       ; 8 chars

KernelFileName                  db 'KERNEL  BIN'    ; 11 chars
NonDataSectors                  dw 0x0000

; Entry point of bootloader
Main:
    
    ; Disable interrupts (will be enabled again during kernel initialization sequence)
    cli
    
    
    ; Clear DF flag in EFLAGS register
    cld
    
    ; Clear segment registers
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Set stack pointer to be directly under bootloader
    ; About 30 KiB of memory is free here
    ; https://wiki.osdev.org/Memory_Map_(x86)
    mov esp, 0x7C00
    mov ebp, esp
    
    ; Initialize data segment register
    mov ax, cs
    mov ds, ax

    ; Save device number (DL register) to memory
    mov [INT13Scratchpad], dl

    ; Save non data sectors count to memory
    call GetNonDataSectorsCount    ; ret cx = 0x0021 ( 33 )
    mov [NonDataSectors], cl
    
   

    ; Reset floppy before read
    call ResetFloppy

    ;floppy_loop:
        ; Load FAT12 non data sectors
        ; Number of sectors to read
        mov al, [NonDataSectors] ; 33 섹터로드

        ; We don't want load first sector with bootloader again
        ; 첫 섹터는 뺀다 고로 32섹터
        dec al

        ; Sector number
        mov cl, 1

        ; Segment
        xor bx, bx
        mov es, bx

        ; Offset
        mov bx, 0x7E00
        ; 0x00007e00에 32섹터 로드 시작

        call LoadFloppyData

    ;    cmp ah, 0x61
    ;    je floppy_out

    ;floppy_wait_keyboard:
    ;    mov ah, 1
    ;    int 0x16
    ;    jz floppy_wait_keyboard
    ;    mov ah, 0
    ;    int 0x16
    ;    cmp al, 'n'
    ;    jne floppy_wait_keyboard
    ;    jmp floppy_loop

    ;floppy_out:

    ; Get kernel first sector and store it in ax register
    call GetFirstKernelSector

    ; Load all kernel sectors
    call LoadKernel

    ; Enter protected mode and jump to the loaded kernel
    call JumpToKernel

    ; We don't expect to be here ever
    JMP $

; Input: nothing
; Output:
;   - ah - disk operation status (http://stanislavs.org/helppc/int_13-1.html)
;   - cf - 1 if successful, 0 if error
ResetFloppy:
    ; Function name: Reset Disk System
    mov ah, 0

    int 0x13
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

; Input:
;   - al - sectors count
;   - cl - sector number
;   - es - segment
;   - bx - offset
; Output:
;   - ah - status (http://stanislavs.org/helppc/int_13-1.html)
;   - al - number of sectors read
;   - cf - 1 if successful, 0 if error
LoadFloppyData:
    push ebp
    mov ebp, esp
    
    movzx si, al    ; al을 si로 복사하는데 si의 상위 Bit는 0으로 채워짐
    ; 결국 al 만큼 반복하겠다

    ; Function name: Read Disk Sectors
    mov ax, 0x0201  ; 읽기 / 1 섹터 ah:x02 al:x01

    ; Get sector (cl), track (ch) and head (dh)
    ; ax = [ebp - 2]
    push ax
    ; bx = [ebp - 4]
    push bx
    ; cx = [ebp - 6]
    push cx

loadfloppy_loop:
    mov bx, cx
    call GetCHS
    mov ax, [ebp - 2]
    mov bx, [ebp - 4]
    
    ; Drive number
    mov dl, [INT13Scratchpad]

    int 0x13
    dec si
    jz loadfloppy_exit

    add bx, 0x200
    mov [ebp - 4], bx
    inc word [ebp - 6]
    mov cx, [ebp - 6]
    jmp loadfloppy_loop

loadfloppy_exit:

    mov esp, ebp
    pop ebp

    ret
    
; Input: nothing
; Output:
;   cx - number of floppy sectors with metadata
GetNonDataSectorsCount:
    ; Boot sector
    mov cx, 1

    ; FAT sectors (number of FATs * sectors per FAT)
    xor ax, ax
    mov al, [FileAllocationTables]  ; 0x02
    mov dx, [LogicalSectorsPerFAT]  ; 0x0009
    mul dx
    add cx, ax  ; cx(0x13 : dec19) = cx(1) + ( ax(0x02) * dx(0x0009) )

    ; Root directory sectors (number of root entries * 32 / bytes per sector)
    mov ax, [MaxRootDirectoryEntries]   ; 0x00E0
    mov dx, 32
    mul dx  ; ax = 0x00E0 * 32 = 0x1C00
    xor dx, dx
    mov bx, [BytesPerLogicalSector]     ; 512
    div bx  ; ax = 0x1C00 / 0x0200 = 0x0E
    add cx, ax 
    ; cx = cx( 0x13 ) + 0x0E =  0x0021
    ret

; Input: nothing
; Output:
;   ax - kernel first sector number
GetFirstKernelSector:
    ; Set initial offset of non data sectors loaded into memory
    mov cx, 0x7E00

    ; Add size of both File Allocations Tables to get root directory offset
    mov al, [FileAllocationTables]  ; 2
    mov dx, [LogicalSectorsPerFAT]  ; 9
    mul dx

    mov dx, [BytesPerLogicalSector] ; 512
    mul dx  ; ax = 2 * 9 * 512 = 9216 (0x2400)

    ; Now cx contains offset of root directory
    add cx, ax ; 0x7E00 + 0x2400 = 0xA200 ( 41,472 ) => 18 Sector 이후의 메모리 (FAT0,1 건너뜀)

    GetFirstKernelSector_BeginCompareLoop:
    ; Load name of entry and expected kernel name offsets
    mov si, cx
    mov di, KernelFileName

    ; Current string index will be stored in dx register
    xor dx, dx

    GetFirstKernelSector_CompareLoop:
    ; Check if whole string has been compared
    cmp dx, 10
    je GetFirstKernelSector_KernelFound

    ; Compare chars from both strings
    mov ax, [si]
    mov bx, [di]
    ;DEBUG
    ;push cx
    
    ;mov ch, 0x0c
    ;mov cl, al

    ;push ax
    ;DispLoop:
    ;xor ax, ax
    ;mov ah, 1
    ;int 0x16
    ;jz DispLoop
    ;mov ah, 0
    ;int 0x16
    ;cmp al, 'n'
    ;jne DispLoop

    ;mov [es: 0], cx

    ;pop ax
    ;pop cx
;DEBUG END
    cmp ax, bx

    ; If chars aren't equal, go to next entry
    jne GetFirstKernelSector_FilenameNotEqual

    ; Increment index and char addresses
    inc dx
    inc si
    inc di

    ; Go to next iteration
    jmp GetFirstKernelSector_CompareLoop

    GetFirstKernelSector_FilenameNotEqual:
    ; Go to next entry
    add cx, 0x20
    jmp GetFirstKernelSector_BeginCompareLoop

    GetFirstKernelSector_KernelFound:
    ; Move address with sector number to bx register due to Intel limitations and then move sector number to ax register
    ; https://stackoverflow.com/questions/1797765/assembly-invalid-effective-address
    mov bx, cx
    add bx, 0x1A
    mov ax, [bx]

    ret

; Input:
;   ax - sector index
; Output:
;   ax - sector value
GetSectorValue:
    ; Multiple sector index by 3/2 to get physical byte address
    mov bx, 3
    mul bx

    mov bx, 2
    xor dx, dx
    div bx

    ; Set initial FAT offset
    mov bx, 0x7E00

    ; Add FAT offset to the calculated byte address
    add bx, ax

    ; If remainder is equal to 0, then index is even, otherwise it's odd
    cmp dx, 0
    je GetSectorValue_EvenIndex
    jne GetSectorValue_OddIndex

    GetSectorValue_EvenIndex:
    ; Even sector has pattern LL _H __
    mov ah, [bx + 1]
    mov al, [bx]
    and ax, 0x0FFF
    jmp GetSectorValue_End

    GetSectorValue_OddIndex:
    ; Odd sector has pattern __ L_ HH
    dec bx
    mov ah, [bx + 2]
    mov al, [bx + 1]
    shr ax, 4

    GetSectorValue_End:
    ret

; Input:
;   ax - initial kernel sector
; Output:
;   bx - loaded kernel sectors count
LoadKernel:
    push ebp
    mov  ebp, esp

    ; Push initial kernel sector
    push ax

    ; Push initial loaded sectors count
    xor bx, bx
    push bx
    
    ; Initial segment
    mov bx, 0x0000
    mov es, bx

    LoadKernel_LoadNextSector:
    ; Offset (current sector index * 512 bytes)
    mov ax, [ebp - 4]
    mov dx, 0x200
    mul dx
    mov bx, ax
    add bx, 0xF000
    
    cmp bx, 0
    jne LoadKernel_Increment

    mov ax, es
    add ax, 0x1000
    mov es, ax

    LoadKernel_Increment:
    ; Number of sectors to read
    mov al, 1

    ; Sector number
    mov cx, [ebp - 2]
    add cx, [NonDataSectors]
    sub cx, 2

    ; Load kernel sector
    call LoadFloppyData

    ; Increment loaded sectors count
    mov ax, [ebp - 4]
    inc ax
    mov [ebp - 4], ax

    ; Get next sector number
    mov ax, [ebp - 2]
    call GetSectorValue
    mov [ebp - 2], ax

    ; Check if the current sector was the last (end-of-chain marker)
    cmp ax, 0x0FF0
    jl LoadKernel_LoadNextSector

    ; Return loaded kernel sectors count in bx
    mov bx, [ebp - 4]

    mov esp, ebp
    pop ebp

    ret

; Input: nothing
; Output: nothing
JumpToKernel:
    jmp 0x0000:0xF000

times 510 - ($ - $$) db 0
dw 0xAA55