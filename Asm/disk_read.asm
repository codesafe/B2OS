
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
	; ax - 찾은 kernel file First Logical Cluster
    ret

;===============================================================

; Input:
;   ax - initial kernel sector
; Output:
;   bx - loaded kernel sectors count
load_kernel:
    push ebp			; bp backup
    mov  ebp, esp		; sp = bp , stack 초기화

    ; ax = bp-2	: 찾은 kernelfile 시작 sector
    push ax				

    xor bx, bx
	; bx = bp-4 : sector 읽기 카운트 초기화
    push bx				
    
    ; segment는 0x0000 부터
    mov bx, 0x0000
    mov es, bx

load_next_sector:
    ; Offset (current sector index * 512 bytes)
    mov ax, [ebp - 4]   ; push된 bx 최초에는 0 : kernel read count
    mov dx, 0x200		; 512 
    mul dx				; [ebp - 4] * 512
    mov bx, ax
    add bx, [KERNEL_LOCATION]		; 0xF000 + ([ebp - 4] * 512)
    
    cmp bx, 0
    jne load_kernel_inc
	; offset의 한계를 넘어가면 segment를 증가해야함 ( 맞나? )
    mov ax, es
    add ax, 0x1000
    mov es, ax

load_kernel_inc:
    mov al, 1	; 1 sector씩 read

    ; Sector number
    mov cx, [ebp - 2]   ; push된 kernel file 시작 logical cluster
    add cx, FAT12_SECTOR_COUNT+BOOT_SECTOR_COUNT
    sub cx, 2			; Directory의 값중 first logical cluster 값이 2이면 물리적 Sector 33에서 시작
						; 만일 first logical cluster가 0이라면 이것은 Root directory를 말함
    call read_disk

    mov ax, [ebp - 4]
    inc ax					; 다음 sector 읽기위해 kernel read count 증가
    mov [ebp - 4], ax		; 증가한 카운트 메모리에 update

    ; Get next sector number
    mov ax, [ebp - 2]
    call get_sector_value	; FAT Chain에서 다음 sector 값을 얻는다
    mov [ebp - 2], ax		; 다음 logical cluster update

    cmp ax, FAT_END_OF_CHAIN	; (0x0FF0) chain의 끝인가??
    jl load_next_sector

    ; Return loaded kernel sectors count in bx
    mov bx, [ebp - 4]

    mov esp, ebp
    pop ebp

    ret


;===============================================================

; Input:
;   ax - sector index
; Output:
;   ax - sector value
get_sector_value:
    ; Multiple sector index by 3/2 to get physical byte address
    mov bx, 3
    mul bx

    mov bx, 2
    xor dx, dx
    div bx

    ; Set initial FAT offset
    mov bx, [FAT12_LOCATION]	; 0x7E00

    ; Add FAT offset to the calculated byte address
    add bx, ax

    ; If remainder is equal to 0, then index is even, otherwise it's odd
    cmp dx, 0
    je even
    jne odd

even :
    ; Even sector has pattern LL _H __
    mov ah, [bx + 1]
    mov al, [bx]
    and ax, 0x0FFF
    jmp get_sector_value_done

odd :
    ; Odd sector has pattern __ L_ HH
    dec bx
    mov ah, [bx + 2]
    mov al, [bx + 1]
    shr ax, 4

get_sector_value_done:
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

