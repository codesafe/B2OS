
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
BytesPerSector		dw 0x0200		; 2 byte
;----------------------------------------------------------------------------------------


; BPB (BIOS Parameter Block)
; https://m.blog.naver.com/rh0969/133710744
; https://wiki.osdev.org/FAT
;----------------------------------------------------------------------------------------
; 13 (클러스터의 크기(1섹터로 해야 함))
SectorsPerCluster	db 0x01			; 1 byte
;----------------------------------------------------------------------------------------
; 14 ( FAT가 어디에서 시작될까 (보통은 1섹터째부터) )
ReservedSectorCount	dw 0x0001		; 2 byte
;----------------------------------------------------------------------------------------
; 16 (Storage media의 FAT번호 대부분이 0x02)
FileAllocationTable	db 0x02			; 1 byte
;----------------------------------------------------------------------------------------
; 17 ( 루트 디렉토리 영역의 크기(보통은 224엔트리로 한다) )
RootDirectoryEntry	dw 0x00E0		; 2 byte
;----------------------------------------------------------------------------------------
; 19 ( FAT12 볼륨에 할당된 전체 섹터의 개수 : 2880 )
TotalSector			dw 0x0B40		; 2 byte
;----------------------------------------------------------------------------------------
; 21 ( F0 : 1.44MB 플로피 )
; https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system#BPB20_OFS_0Ah
MediaType           db 0xF0			; 1 byte
;----------------------------------------------------------------------------------------
; 22 ( FAT영역 길이(9섹터로 해야 함) )
SectorsPerFAT		dw 0x0009		; 2 byte
;----------------------------------------------------------------------------------------
; 24 ( 1트럭에 몇개의 섹터가 있을까(18로 해야 함) )
SectorsPerTrack		dw 0x0012		; 2 byte
;----------------------------------------------------------------------------------------
; 26 ( 헤드 수(2로 해야 함), Number of heads or sides on the storage media )
Head				dw 0x0002		; 2 byte
;----------------------------------------------------------------------------------------
; 28 ( Hidden Sector, 파티션을 사용하지 않기 때문에 여기는 반드시 0 )
HiddenSector		dd 0x00000000	; 4 byte
;----------------------------------------------------------------------------------------
; 32
; Large sector count. This field is set if there are more than 65535 sectors in the volume, 
; resulting in a value which does not fit in the Number of Sectors entry at 0x13.
TotalHiddenSectors	dd 0x00000000	; 4 byte
;----------------------------------------------------------------------------------------
; 36
; 드라이브 번호다. BIOS Interrupt 0x13 호출 시 반환되는 값과 동일하다. 
; 플로피 디스크의 경우 0x00이고, 하드 디스크의 경우 0x80이다.
; - INT 13h: DOS 같은 Real-mode OS에서 INT 13h를 호출하면 CHS 주소 지정 
; 기반의 디스크 읽기/쓰기 작업을 수행하는 Low-level disk service용 ROM-BIOS 코드로 점프한다.
; [5] 이 때 DL 레지스터에 0x00을 넣으면 첫번째 플로피 디스크(Drive A:)에 대한 서비스라는 의미고, 
; 0x80을 넣으면 첫번재 하드 디스크에 대한 서비스라는 의미다. 즉 INT 13h를 호출할 때 어느 미디어에 
; 대한 인터럽트 서비스인지를 나타내는 값이다.
DriveNumber			db 0x00			; 1 byte
;----------------------------------------------------------------------------------------
; 37 ( Reserved / 다른 용도로 사용 )
INT13Scratchpad		db 0x00			; 1 byte
;----------------------------------------------------------------------------------------
; 38 ( 	Signature (must be 0x28 or 0x29). )
BootSignature		db 0x29			; 1 byte
;----------------------------------------------------------------------------------------
; 39 (볼륨 시리얼 번호)
VolumeID			dd 0x12345678	; 4 byte
;----------------------------------------------------------------------------------------
; 43  볼륨 이름
VolumeLabel			db 'B2OS-FAT   '    ; 11 chars
;----------------------------------------------------------------------------------------
; 54 ( FAT 이름 )
FileSystemType		db 'FAT12   '       ; 8 chars
; 끝
;----------------------------------------------------------------------------------------



; 62 ~ 510
; 여기서 부터 Boot Code
boot_first:
    ; 나중을 위하여 boot drive를 기억 ( 0 : floppy , 1 : floppy2, 0x80 : hdd, 0x81 : hdd2 )
    ; 이것은 Bios에서 정해지는 값
	mov [BOOT_DISK], dl 

	cli	; stop interrupt
	xor ax, ax	; 0 reset
	mov ds, ax
	mov es, ax	; [es:bx]
	mov ss, ax
	mov sp, 0x7c00
	mov bp, 0x9000	; stack은 여기부터
	sti	; enable interrupt

	; clear screen										
	mov ah, 0x0
	mov al, 0x3
	int 0x10                ; text mode

	mov bx, BOOT_SECOND ;[es:bx] 메모리 위치에 disk 읽어 로딩

	mov al, 0x02    ; 읽을 sector 갯수 ; sector 몇개 읽을것인가? ( 작으면 리부팅됨 )
	; floppy의 1sector는 512 byte	
    mov cl, 0x02    ; sector 번호 (boot loader가 첫번째 512byte, 2번째 sector)
	mov ch, 0x00    ; cylinder 번호
	mov dh, 0x00    ; head 번호
	call read_disk

	jmp BOOT_SECOND

%include "Asm/disk_read.asm"

BOOT_SECOND	equ	0x7e00


times 510-($-$$) db 0	; Boot loader를 512에 맞춤 (마지막 aa55를 빼고 코드부분이외 나머지를 0으로 총510바이트 + aa55 = 512)
dw 0xaa55