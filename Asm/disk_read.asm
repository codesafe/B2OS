
[BITS 16]

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

; @fn _fetchRootDirectory: this function is used to read the FAT12 root directory
; ES:BX -> data buffer for root directory sector
_fetchRootDirectory:
	; compute the size of the root directory and store in cx
	xor cx,cx
	xor dx,dx
	mov ax,32                       ;_ROOT_ENTRY_SIZE = 33 - 1
	mul word [RootDirectoryEntry]
	div word [BytesPerSector]
	xchg ax,cx

	; compute the location of the root directory
	mov al,byte [FileAllocationTable]
	mul word [SectorsPerFAT]
	add ax,word [ReservedSectorCount]

	; store the end of the root directory
	mov word [_firstDataSector],ax
	add word [_firstDataSector],cx

	call _readSectors
	ret

;===============================================================

; @fn _parseRootDirectory: this function will parse the root directory for the
; file by the name indicated in DS:SI. The filename must be in MS-DOS 8.3 format
; DS:SI -> file name pointer
; ES:DI -> root directory structure
; CX -> root directory size

_parseRootDirectory:

	mov cx,word [RootDirectoryEntry]

.parseLoop:

	; save important registers
	push si
	push di
	push cx

	; search for the filename
	mov cx,11           ; _FILENAME_LEN = 11
	rep cmpsb

	; restore the registers before the next attempt
	pop cx
	pop di
	pop si

	je .found		; filename was found!

	; check the next entry while there are entries left
	add di,32       ; _ROOT_ENTRY_SIZE = 33 -1
	loop .parseLoop

	; no more entries; go to error condition return
	stc
	jmp .done

.found:
	clc								; the file was found, clear the error status
	mov dx,word [di+0x1a]			; this is our starting sector
	mov word [_currentCluster],dx	; store the starting sector

.done:
	ret

;===============================================================

; @fn _readStage2Image: Read the second stage into memory
; This function expects the following conditions:
; ES:BX => destination buffer for the FAT
; _currentCluster => starting cluster number of the file
_readStage2Image:

	; compute the size of the FAT
; %ifdef _LOAD_BOTH_FAT_COPIES
; 	xor ax,ax
; 	mov al,byte [FileAllocationTable]
; 	mul word [SectorsPerFAT]    ; 9 sector
; 	xchg cx,ax
; %else
	mov cx,word [SectorsPerFAT]
;%endif

	; compute the location of the FAT (just above the reserved sectors)
	mov ax,word [ReservedSectorCount]

	; read the FAT into memory at ES:BX
	mov word [_fatBuffer],bx
	call _readSectors

	; set up ES and BX to receive the image file
	mov ax,_OS_LOAD_SEGMENT
	mov es,ax
	mov bx,_OS_LOAD_OFFSET
	push bx

.readFileLoop:
	mov ax,word [_currentCluster]
	pop bx								; restore our buffer
	call clustertolba

	xor cx,cx
	mov cl,byte [SectorsPerCluster]
	call _readSectors
	jc .done
	push bx

	; this is the part where we find the next cluster!
	mov ax,word [_currentCluster]
	mov cx,ax
	mov dx,ax
	shr dx,0x0001
	add cx,dx
	mov bx,[_fatBuffer]
	add bx,cx
	mov dx,word [bx]
	test ax,0x0001
	jz .evenCluster

.oddCluster:

	shr dx,0x04
	jmp .clusterDone

.evenCluster:

	and dx,0x0fff

.clusterDone:

	mov word [_currentCluster],dx
	cmp dx,0xff8
	jb .readFileLoop
	clc

.done:

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

