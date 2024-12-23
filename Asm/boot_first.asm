; |------------|---------|------------|------------|----------------|---------|------------|------------|---------|---------|------------|------------|------------|------------|
; |   1 KiB    |  256 B  |   21 KiB   |   1 KiB    |     4 KiB      |  3 KiB  |   512 B    |   28 KiB   | 578 KiB |  1 KiB  |  383 KiB   |   15 MiB   |   1 MiB    |   4 MiB    |
; | Interrupts |   BDA   | Floppy DMA | Memory map | Page Directory |  Boot2  | Bootloader | FAT12 data |  Kernel |  EBDA   | Video, ROM |  Reserved  |   Stack    | Page Table |
; |            |         |            |            |                |         |            |            |         |         |            |            |            |            |
; |  0x00000   | 0x00400 |  0x00500   |  0x05C00   |    0x06000     | 0x07000 |  0x07C00   |  0x07E00   | 0x0F000 | 0x9FC00 |  0xA0000   | 0x00100000 | 0x01000000 | 0x01100000 |
; |  0x003FF   | 0x004FF |  0x05BFF   |  0x05FFF   |    0x06FFF     | 0x07BFF |  0x07DFF   |  0x0EFFF   | 0x9FBDD | 0x9FFFF |  0xFFFFF   | 0x00FFFFFF | 0x010FFFFF | 0x014FFFFF |
; |------------|---------|------------|------------|----------------|---------|------------|------------|---------|---------|------------|------------|------------|------------|
; {                                        Segment 1                                                       }{                            Segment 2 - n                          }

;%define READ_PROGRESS
%define LOG

[BITS 16]
[org 0x7c00]

; 0 ~ 2 ( EB 3C 90 )
jmp boot_first
nop

; FAT12 Header
;----------------------------------------------------------------------------------------
; 3 ( OEM Id, 8 byte )
OEM_ID				db 'B2OS    '	; 8 byte
;----------------------------------------------------------------------------------------
; 11 ( 1섹터의 크기(512로 해야 함) )
BytesPerSector		dw 512		; 2 byte
;----------------------------------------------------------------------------------------


; BPB (BIOS Parameter Block)
; https://m.blog.naver.com/rh0969/133710744
; https://wiki.osdev.org/FAT
;----------------------------------------------------------------------------------------
; 13 (클러스터의 크기(1섹터로 해야 함))
SectorsPerCluster	db 1			; 1 byte
;----------------------------------------------------------------------------------------
; 14 ( FAT가 어디에서 시작될까 (보통은 1섹터째부터) )
ReservedSectorCount	dw 1		; 2 byte
;----------------------------------------------------------------------------------------
; 16 (Storage media의 FAT번호 대부분이 0x02)
FileAllocationTable	db 2			; 1 byte
;----------------------------------------------------------------------------------------
; 17 ( 루트 디렉토리 영역의 크기(보통은 224엔트리로 한다) )
RootDirectoryEntry	dw 224		; 2 byte
;----------------------------------------------------------------------------------------
; 19 ( FAT12 볼륨에 할당된 전체 섹터의 개수 : 2880 )
TotalSector			dw 2880		; 2 byte
;----------------------------------------------------------------------------------------
; 21 ( F0 : 1.44MB 플로피 )
; https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system#BPB20_OFS_0Ah
MediaType           db 0xF0			; 1 byte
;----------------------------------------------------------------------------------------
; 22 ( FAT영역 길이(9섹터로 해야 함) )
SectorsPerFAT		dw 9		; 2 byte
;----------------------------------------------------------------------------------------
; 24 ( 1트럭에 몇개의 섹터가 있을까(18로 해야 함) )
SectorsPerTrack		dw 18		; 2 byte
;----------------------------------------------------------------------------------------
; 26 ( 헤드 수(2로 해야 함), Number of heads or sides on the storage media )
Head				dw 2		; 2 byte
;----------------------------------------------------------------------------------------
; 28 ( Hidden Sector, 파티션을 사용하지 않기 때문에 여기는 반드시 0 )
HiddenSector		dd 0	; 4 byte
;----------------------------------------------------------------------------------------
; 32
; Large sector count. This field is set if there are more than 65535 sectors in the volume, 
; resulting in a value which does not fit in the Number of Sectors entry at 0x13.
TotalHiddenSectors	dd 0	; 4 byte
;----------------------------------------------------------------------------------------
; 36
; 드라이브 번호다. BIOS Interrupt 0x13 호출 시 반환되는 값과 동일하다. 
; 플로피 디스크의 경우 0x00이고, 하드 디스크의 경우 0x80이다.
; - INT 13h: DOS 같은 Real-mode OS에서 INT 13h를 호출하면 CHS 주소 지정 
; 기반의 디스크 읽기/쓰기 작업을 수행하는 Low-level disk service용 ROM-BIOS 코드로 점프한다.
; [5] 이 때 DL 레지스터에 0x00을 넣으면 첫번째 플로피 디스크(Drive A:)에 대한 서비스라는 의미고, 
; 0x80을 넣으면 첫번재 하드 디스크에 대한 서비스라는 의미다. 즉 INT 13h를 호출할 때 어느 미디어에 
; 대한 인터럽트 서비스인지를 나타내는 값이다.
DriveNumber			db 0			; 1 byte
;----------------------------------------------------------------------------------------
; 37 ( Reserved / 다른 용도로 사용 )
DriveNo				db 0			; 1 byte
;----------------------------------------------------------------------------------------
; 38 ( 	Signature (must be 0x28 or 0x29). )
BootSignature		db 0x29			; 1 byte
;----------------------------------------------------------------------------------------
; 39 (볼륨 시리얼 번호)
VolumeID			dd 0x12345678	; 4 byte
;----------------------------------------------------------------------------------------
; 43  볼륨 이름
VolumeLabel			db 'B2OS-DISK  '    ; 11 chars
;----------------------------------------------------------------------------------------
; 54 ( FAT 이름 )
FileSystemType		db 'FAT12   '       ; 8 chars
; 끝
;----------------------------------------------------------------------------------------

; TODO
; 1. FAT 정보를 0x7e00로 Load ( 33 Sector - 1 : Bootloader에서 읽은 1 sector 제외 )
; 2. 로드된 FAT 정보에서 Kernel.bin 파일을 찾는다 (메모리에서는 .은 없다 )
; 3. 찾은 Kernel 로드
;	1 sector씩 크기만큼
; 4. Kernel로 Jump (jmp 0x0000:0xF000)
; 5. Protected Mode 진입
;	GDT, Interrupt 설정	
; 6. kmain
; 7. Interrupt init
; 8. memory manager init
; 9. timer init
; fat12에서 file list / load 구현
;	floppy list / load
; shell 구현
; 32bit memory에 app load
; video mode 구현



; 62 ~ 510
; 여기서 부터 Boot Code
boot_first:
    cli
    cld
    ; Clear segment registers
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, 0x7C00
    mov ebp, esp
    
    mov ax, cs
    mov ds, ax
    mov [BOOT_DISK], dl
	sti

	call reset_disk

%ifdef LOG
	pusha
	mov si, load_fat_str
	call print_str
	popa
%endif

;==========================================================================================
	mov cl, FAT12_SECTOR_COUNT ; 33 섹터로드 - 1 boot
	mov al, 1
	xor bx, bx
	mov es, bx
	mov bx, FAT12_LOCATION	; 0x07e00에 32섹터 로드 시작
	call read_disk

;==========================================================================================

	mov bx, BOOT2FILENAME
	call search_file

	; load boot2.bin to 0x7E00
	mov cx, BOOT2_LOCATION
	call load_file

	; jump to boot2
	jmp BOOT2_LOCATION
	jmp $


%include "Asm/disk_read.asm"

FAT12_LOCATION			equ	0x7E00
BOOT2_LOCATION			equ 0x7000
FAT12_SECTOR_COUNT		equ 32
BOOT_SECTOR_COUNT		equ 1
FAT_END_OF_CHAIN		equ 0x0FF0

BOOT2FILENAME		db 'BOOT2   BIN'    ; 11 chars
BOOT_DISK			db 0 	; Boot device number

load_fat_str		db 'LOAD FAT', 0x0a, 0x0d, 0
disk_read_ok		db 'READ OK', 0x0a, 0x0d, 0
disk_read_error		db 'READ ERR',0x0a, 0x0d, 0

; data section for floppy operations
_cylinder   	db 0x00
_head       	db 0x00
_sector     	db 0x00


times 510-($-$$) db 0	; Boot loader를 512에 맞춤 (마지막 aa55를 빼고 코드부분이외 나머지를 0으로 총510바이트 + aa55 = 512)
dw 0xaa55