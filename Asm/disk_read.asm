
[BITS 16]

%define _OS_LOAD_SEGMENT 0x0050
%define _OS_LOAD_OFFSET 0x0000

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

; @fn clusterToLBA: Convert a cluster value to a linear block address
clustertolba:

	sub ax,0x02								; make the cluster zero-based
	xor cx,cx								; clear cx for operations
	mov cl,byte [SectorsPerCluster]	        ; store the sectors per cluster in cl =>  0x01
	mul cx									; multiply the cluster number by the sectors per cluster
	add ax,word [_firstDataSector]			; and add the starting sector offset
	ret


;===============================================================

; lba : ax
; cx : read count
read_disk:
	mov di,0x5			; 오류시 재시도 횟수

read_loop:
	; we need to preserve these between reads
	push ax
	push bx
	push cx

	call lbatochs
	;mov ax,0x0201				; ah = bios function 2, al = read 1 sector
	mov ah, 0x02
	mov al, 0x01
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
	mov si,disk_read_ok
	call print_str
	ret


read_error:
	mov si,disk_read_error
	call print_str
	cli
	jmp $
	
;===============================================================

; Load된 FAT의 Directory에서 Kernel.bin 파일 검색
search_kernel_file :
    mov cx, [FAT12_LOCATION]	; FAT 시작지점

    mov al, [FileAllocationTable]  	; 2
    mov dx, [SectorsPerFAT]  		; 9
    mul dx

    mov dx, [BytesPerSector] 		; 512
    mul dx  ; ax = 2 * 9 * 512 = 9216 (0x2400)
    add cx, ax ; 0x7E00 + 0x2400 = 0xA200 ( 41,472 ) => 18 Sector 이후의 메모리 (FAT0,1 건너뜀)

search_loop:
    mov si, cx
    mov di, KERNELFILENAME  ; kernel bin (kernel.bin)

    xor dx, dx
compare_loop:
    cmp dx, 10  ; 11글자 비교 0 ~ 10
    je kernel_found

    ; Compare chars
    mov ax, [si]
    mov bx, [di]

    cmp ax, bx
    jne filenot_match

    inc dx ; 증가하면서 비교
    inc si
    inc di
    jmp compare_loop	; 계속 비교

filenot_match:
    add cx, 0x20    	; 14 sector * 512 / 224개 = 32 byte ( filename 1개당 32 byte )
    jmp compare_loop	; 못찾았으니 다음 file 검색

kernel_found:
    ; Move address with sector number to bx register due to Intel limitations and then move sector number to ax register
    ; https://stackoverflow.com/questions/1797765/assembly-invalid-effective-address
    mov bx, cx
    add bx, 0x1A	; 26 -> Directory 정보에서 First Logical Cluster는 26 Byte부터 있다 (2byte)
					; https://www.sqlpassion.at/archive/2022/03/03/reading-files-from-a-fat12-partition/
    mov ax, [bx]
	; ax - 찾은 kernel file sector number
    ret

;===============================================================

; Input:
;   ax - initial kernel sector
; Output:
;   bx - loaded kernel sectors count
load_kernel:
    push ebp			; bp backup
    mov  ebp, esp		; sp = bp , stack 초기화

    ; Push initial kernel sector
    push ax				; ax = bp-2

    ; Push initial loaded sectors count
    xor bx, bx
    push bx				; bx = bp-4
    
    ; Initial segment
    mov bx, 0x0000
    mov es, bx

load_next_sector:
    ; Offset (current sector index * 512 bytes)
    mov ax, [ebp - 4]   ; push된 bx = 0
    mov dx, 0x200		; 512 
    mul dx				; [ebp - 4] * 512
    mov bx, ax
    add bx, [KERNEL_LOCATION]		; 0xF000 + ([ebp - 4] * 512)
    
    cmp bx, 0
    jne LoadKernel_Increment
	; offset의 한계를 넘어가면 segment를 증가해야함 ( 맞나? )
    mov ax, es
    add ax, 0x1000
    mov es, ax

LoadKernel_Increment:
    ; Number of sectors to read
    mov al, 1

    ; Sector number
    mov cx, [ebp - 2]   ; push된 ax
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
    jl load_next_sector

    ; Return loaded kernel sectors count in bx
    mov bx, [ebp - 4]

    mov esp, ebp
    pop ebp

    ret


;===============================================================

; data section for floppy operations
_cylinder   	db 0x00
_head       	db 0x00
_sector     	db 0x00

_fatBuffer			dw 0x00
_firstDataSector	dw 0x00
_currentCluster 	dw 0x00


%include "./Asm/print16.asm"

%ifdef READ_PROGRESS
read_progress		db '.', 0
%endif

disk_read_ok		db 'READ OK', 0x0a, 0x0d, 0
disk_read_error		db 'READ ERR',0x0a, 0x0d, 0

