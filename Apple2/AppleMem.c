
/*
	MOS 6502 CPU Emulator
*/

#include "AppleMem.h"
#include "AppleDevice.h"
#include "../Src/memory.h"
#include "../Src/console.h"

bool LCWritable = true;
bool LCReadable = false;
bool LCBank2Enable = true;
bool LCPreWriteFlipflop = false;

BYTE ram[RAMSIZE];  // 48K of ram in $000-$BFFF
BYTE rom[ROMSIZE];  // 12K of rom in $D000-$FFFF
BYTE lgc[LGCSIZE];  // Language Card 12K in $D000-$FFFF
BYTE bk2[BK2SIZE];  // bank 2 of Language Card 4K in $D000-$DFFF
BYTE sl6[SL6SIZE];  // P5A disk ][ PROM in slot 6

BYTE* memory;

void mem_Create()
{
#if 0
	memory = new BYTE[MEMORY_SIZE];
	memset(memory, 0, MEMORY_SIZE);
#else
	mem_Reset();
#endif
}

void mem_Destroy()
{
#if 0
	delete[] memory;
	memory = NULL;
#endif
}

void mem_Reset()
{
	LCWritable = true;
	LCReadable = false;
	LCBank2Enable = true;
	LCPreWriteFlipflop = false;

	memset(ram, 0, RAMSIZE);
	memset(rom, 0, ROMSIZE);
	memset(lgc, 0, LGCSIZE);
	memset(bk2, 0, BK2SIZE);
	memset(sl6, 0, SL6SIZE);
}


BYTE mem_ReadByte(WORD address)
{
	if (address < (WORD)RAMSIZE)
		return ram[address];                                                        // RAM

	if (address >= (WORD)ROMSTART) {
		if (!LCReadable)
			return rom[address - (WORD)ROMSTART];                                           // ROM

		if (LCBank2Enable && (address < (WORD)0xE000))
			return bk2[address - BK2START];                                           // BK2

		return lgc[address - LGCSTART];                                             // LC
	}

	if ((address & 0xFF00) == SL6START)
	{
		//k_print("DISK");
		return sl6[address - SL6START];  // disk][
	}

	if ((address & 0xF000) == 0xC000)
	{
		// k_print("SOFT");
		// k_printnum(address);
		return (ad_SoftSwitch(address, 0, false));			
	}

	return 0;
}

void mem_WriteByte(WORD address, BYTE value)
{
	if (address < (WORD)RAMSIZE) {
		ram[address] = value;                                                       // RAM
		return;
	}

	if (LCWritable && (address >= (WORD)ROMSTART)) {
		if (LCBank2Enable && (address < (WORD)0xE000)) {
			bk2[address - BK2START] = value;                                          // BK2
			return;
		}
		lgc[address - LGCSTART] = value;                                            // LC
		return;
	}

	if ((address & 0xF000) == 0xC000)
	{
		ad_SoftSwitch(address, value, true);
		return;
	}
}

WORD mem_ReadWord(WORD addr)
{
	BYTE m0 = mem_ReadByte(addr);
	BYTE m1 = mem_ReadByte(addr + 1);
	WORD w = (m1 << 8) | m0;
	return w;
}

void mem_WriteWord(WORD value, WORD addr)
{
	mem_WriteByte(addr, value >> 8);
	mem_WriteByte(addr + 1, value & 0xFF);
}


// // 프로그램 업로드
// WORD mem_UpLoadProgram(BYTE* code, int codesize)
// {
// 	// 상위 1 WORD는 시작 위치 (PC)
// 	WORD addr = code[0] | (code[1] << 8);
// 	for (int i = 0; i < codesize-2; i++)
// 	{
// 		memory[addr+i] = code[i+2];
// 	}
// 	return addr;
// }

// void mem_UpLoadProgram_Pos(int startPos, BYTE* code, int codesize)
// {
// 	for (int i = 0; i < codesize; i++)
// 		memory[startPos+i] = code[i];
// }


// void mem_UpLoadToRom(BYTE* code)
// {
// 	memcpy(rom, code, ROMSIZE);
// }

void mem_ResetRam()
{
	memset(ram, 0, sizeof(ram));
}

