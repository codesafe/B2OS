
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
; lba->chs 계산기 http://cars.car.coocan.jp/misc/chs2lba.html
; cluster 19에서 잘못 load하는것은 floppy.img가 1.44MB아니고 2.8로 만들어졌기 때문
; lba : ax
; cx : read count
read_disk:
	push ax
	push bx
	push cx

	call lbatochs
	mov ax,0x0201				; ah = bios function 2, al = read 1 sector
	mov ch,byte [_cylinder]
	mov cl,byte [_sector]
	mov dh,byte [_head]
	mov dl,byte [BOOT_DISK]
	int 0x13
	jnc read_ok
	stc             ; set carry flag Error라고 set
	jmp read_done

read_ok:

%ifdef READ_PROGRESS
 	mov si,read_progress
 	call print_str
%endif
	pop cx
	pop bx
	pop ax

	add bx,word [BytesPerSector]	; es:bx -> 1 sector (512)
	inc ax
	loop read_disk          ; cx를 1감소 0 될때까지 반복
	clc                     ; clear carry flag

read_done:
	jc read_error
	; mov si,disk_read_ok
	; call print_str
	ret

read_error:
	mov si,disk_read_error
	call print_str
	cli
	jmp $
	
;===============================================================

; Load된 FAT의 Directory에서 Kernel.bin 파일 검색
; bx : filename
search_file :
    push ebp
    mov  ebp, esp
    push bx     ; bp-2

    mov cx, FAT12_LOCATION	; FAT 시작지점

    mov al, [FileAllocationTable]  	; 2
    mov dx, [SectorsPerFAT]  		; 9
    mul dx

    mov dx, [BytesPerSector] 		; 512
    mul dx  ; ax = 2 * 9 * 512 = 9216 (0x2400)
    add cx, ax ; 0x7E00 + 0x2400 = 0xA200 ( 41,472 ) => 18 Sector 이후의 메모리 (FAT0,1 건너뜀)

search_loop:
    mov si, cx
    ;mov di, KERNELFILENAME  ; kernel bin (kernel.bin)
    mov di, [bp-2]  ; kernel bin (kernel.bin)

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
    jmp search_loop	; 못찾았으니 다음 file 검색

kernel_found:
    ; Move address with sector number to bx register due to Intel limitations and then move sector number to ax register
    ; https://stackoverflow.com/questions/1797765/assembly-invalid-effective-address
    mov bx, cx
    add bx, 0x1A	; 26 -> Directory 정보에서 First Logical Cluster는 26 Byte부터 있다 (2byte)
					; https://www.sqlpassion.at/archive/2022/03/03/reading-files-from-a-fat12-partition/
    mov ax, [bx]

    mov esp, ebp
    pop ebp
	; ax - 찾은 kernel file First Logical Cluster
; %ifdef LOG
; 	pusha
; 	mov si, found_kernel_str
; 	call print_str
; 	popa
; %endif
    ret

;===============================================================

; Input:
;   ax : initial kernel sector 
;   ax로 전달받은 파일에 대하여 FAT에서 읽어 cx 위치에 로드한다
;   cx : load mem location
; Output:
;   bx - loaded kernel sectors count
load_file:
    push ebp			; bp backup
    mov  ebp, esp		; sp = bp , stack 초기화
    push ax				; ax = bp-2	: 찾은 kernelfile 시작 sector

    xor bx, bx
    push bx				; bx = bp-4 : sector 읽기 카운트 초기화
    push cx             ; cx = bp-6 (읽힐 메모리 위치)

    ; segment는 0x0000 부터
    ;mov bx, 0x0000
    mov es, bx

load_next_sector:
    ; Offset (current sector index * 512 bytes)
    mov ax, [ebp - 4]   ; push된 bx 최초에는 0 : kernel read count
    mov dx, 0x200		; 512 
    mul dx				; [ebp - 4] * 512
    mov bx, ax
    ;add bx, KERNEL_LOCATION		; 0xF000 + ([ebp - 4] * 512)
    add bx, [ebp-6]		; 0xF000 + ([ebp - 4] * 512)
    
    cmp bx, 0
    jne load_file_inc
	; offset의 한계를 넘어가면 segment를 증가해야함 ( 맞나? )
    mov ax, es
    add ax, 0x1000
    mov es, ax

load_file_inc:
    mov cx, 1	; 1 sector씩 read

    ; Sector number
    mov ax, [ebp - 2]   ; push된 kernel file 시작 logical cluster
    add ax, FAT12_SECTOR_COUNT+BOOT_SECTOR_COUNT
    sub ax, 2			; Directory의 값중 first logical cluster 값이 2이면 물리적 Sector 33에서 시작
						; 만일 first logical cluster가 0이라면 이것은 Root directory를 말함
    call read_disk

    mov ax, [ebp - 4]
    inc ax					; 다음 sector 읽기위해 kernel read count 증가
    mov [ebp - 4], ax		; 증가한 카운트 메모리에 update

    ; Get next sector number
    mov ax, [ebp - 2]
    ;call get_sector_value	; FAT Chain에서 다음 sector 값을 얻는다
	call get_sector_value_op
    mov [ebp - 2], ax		; 다음 logical cluster update

    cmp ax, FAT_END_OF_CHAIN	; (0x0FF0) chain의 끝인가??
    jl load_next_sector

    ; Return loaded kernel sectors count in bx
    mov bx, [ebp - 4]

    mov esp, ebp
    pop ebp

; %ifdef LOG
; 	pusha
; 	mov si, load_kernel_str
; 	call print_str
; 	popa
; %endif
    ret


;===============================================================

%ifdef OLD_GET_SECTOR
; Input:
;   ax - sector index
; Output:
;   ax - sector value
get_sector_value:
    ; Multiple sector index by 3/2 to get physical byte address
    mov bx, 3
    mul bx		; in (ax값) * 3

    mov bx, 2
    xor dx, dx		; dx = 0
    div bx			; (in*3)/2 => ax는 몫 , dx = 나머지

    ; Set initial FAT offset
    mov bx, [FAT12_LOCATION]	; 0x7E00

    ; Add FAT offset to the calculated byte address
    add bx, ax		; [FAT12_LOCATION] + (in*3)/2의 몫

    ; 나머지가 0이면 짝수, 아니면 홀수
    cmp dx, 0
    je even
    ;jne odd
	jmp odd

even :
    ; Even sector has pattern LL _H __
	; in이 짝수이면 ->  물리적 위치는 위치 1+(3*n)/2의 하위 4비트입니다. & (3*n)/2 8 bits
    mov ah, [bx + 1]
    mov al, [bx]
    and ax, 0x0FFF	; 0000111111111111b 하위 12비트 취함
    jmp get_sector_value_done

odd :
    ; Odd sector has pattern __ L_ HH
	; in이 홀수인 경우 항목의 물리적 위치는 위치 (3*n)/2의 상위 4비트 & 1+(3*n)/2의 8 bits 
    dec bx
    mov ah, [bx + 2]
    mov al, [bx + 1]
    shr ax, 4		; 상위 12비트 취함

get_sector_value_done:
    ret
%endif

; 최적화 버전
; https://github.com/SQLpassion/osdev/blob/3ac59b14f6215a98343b20fd1f7fc1304538706f/main64/boot/functions.asm
; Input:
;   ax - sector index
; Output:
;   ax - sector value
get_sector_value_op:
    mov	cx, ax
	mov dx, ax
	shr dx, 1	; in /2 (2로 나눔)
	add cx, dx	; (in * 3) / 2 의 간략버전
	mov bx, FAT12_LOCATION	; 0x7E00
    add bx, cx
	mov dx, word [bx]			; fat에서 2 바이트 읽음
	test ax, 1					; 홀수 짝수 검사 (in의 최하위 1비트가 1인가??)
	jnz odd_cluster

even_cluster:
    and dx, 0000111111111111b                   ; 하위 12 bits 취함
    jmp get_sector_value_op_done
         
odd_cluster:
    shr dX, 0x0004                              ; 상위 12 bits 취함

get_sector_value_op_done:
	mov ax, dx
	ret

;===============================================================

%include "./Asm/print16.asm"

%ifdef READ_PROGRESS
read_progress		db '.', 0
%endif


