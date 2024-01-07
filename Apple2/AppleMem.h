#ifndef A2_MEMORY_H
#define A2_MEMORY_H

#include "Predef.h"
//#include "Apple2Device.h"

// LC -> Language Card
extern bool LCWritable;
extern bool LCReadable;
extern bool LCBank2Enable;			// bank 2 enabled
extern bool LCPreWriteFlipflop;	// pre-write flip flop

extern BYTE ram[RAMSIZE];  // 48K of ram in $000-$BFFF
extern BYTE rom[ROMSIZE];  // 12K of rom in $D000-$FFFF
extern BYTE lgc[LGCSIZE];  // Language Card 12K in $D000-$FFFF
extern BYTE bk2[BK2SIZE];  // bank 2 of Language Card 4K in $D000-$DFFF
extern BYTE sl6[SL6SIZE];  // P5A disk ][ PROM in slot 6

extern BYTE* memory;

void mem_Create();
void mem_Destroy();
void mem_Reset();

BYTE mem_ReadByte(int addr);
void mem_WriteByte(int addr, BYTE value);
WORD mem_ReadWord(int addr);
void mem_WriteWord(WORD value, int addr);

WORD mem_UpLoadProgram(BYTE *code, int codesize);
void mem_UpLoadProgram_Pos(int startPos, BYTE *code, int codesize);

void mem_UpLoadToRom(BYTE* code);
void mem_ResetRam();

//void Dump(FILE* fp);
//void LoadDump(FILE* fp);

#endif