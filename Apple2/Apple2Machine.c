//	#include "Predef.h"
//#include "rombios.h"
#include "Apple2Machine.h"



void machine_InitMachine()
{
	mem.mem_Create();

	device.InsetFloppy();

	// unset the Power-UP byte
	mem.mem_WriteByte(0x3F4, 0);

	cpu.Reset(mem);

	// dirty hack, fix soon... if I understand why
	mem.mem_WriteByte(0x4D, 0xAA);   // Joust crashes if this memory location equals zero
	mem.mem_WriteByte(0xD0, 0xAA);   // Planetoids won't work if this memory location equals zero

	device.Create(&cpu);
	mem.device = &device;

	Booting();
	//UploadRom();	
}

// 롬을 내장
bool machine_Booting()
{
	memcpy(mem.rom, appleIIrom, ROMSIZE);
	memcpy(mem.sl6, diskII, SL6SIZE);
	cpu.Reset(mem);

	return true;
}

// 롬을 파일에서 로딩
bool machine_UploadRom()
{
	bool ret = false;

	// load the Apple II+ ROM
	BYTE* rom = new BYTE[ROMSIZE];
	FILE* fp = fopen("rom/appleII+.rom", "rb"); 
	if (fp)
	{
#if 0
		fread(rom, ROMSIZE, 1, fp);
		mem.mem_UpLoadProgram(ROMSTART, rom, ROMSIZE);
#else
		fread(mem.rom, ROMSIZE, 1, fp);
#endif
		fclose(fp);
		ret = true;
	}
	delete[] rom;

	// load Apple II+ / Disk II
	BYTE* disk2 = new BYTE[SL6SIZE];
	FILE* disk2fp = fopen("rom/diskII.rom", "rb");
	if (disk2fp)
	{
#if 0
		fread(disk2, SL6SIZE, 1, disk2fp);
		mem.mem_UpLoadProgram(SL6START, disk2, SL6SIZE);
#else
		fread(mem.sl6, SL6SIZE, 1, disk2fp);
#endif
		fclose(disk2fp);
		ret = true;
	}
	delete[] disk2;

	cpu.Reset(mem);
	return ret;
}

void machine_Reset()
{
	mem.mem_Reset();
	cpu.Reboot(mem);
	device.Reset();

	// unset the Power-UP byte
	mem.mem_WriteByte(0x3F4, 0);
	// dirty hack, fix soon... if I understand why
	mem.mem_WriteByte(0x4D, 0xAA);   // Joust crashes if this memory location equals zero
	mem.mem_WriteByte(0xD0, 0xAA);   // Planetoids won't work if this memory location equals zero

	Booting();
}

void machine_Run(int cycle)
{
	if (device.resetMachine)
	{
		Reset();
		return;
	}

	device.UpdateInput();
	cpu.Run(mem, cycle);
	while (1)
	{
		if( device.UpdateFloppyDisk() == false ) 
			break;
		cpu.Run(mem, 10000);
	}
}

void machine_Render(int frame)
{
	device.Render(mem, frame);
}

/*

void machine_FileDroped(char* path)
{
	device.FileDroped(path);
}

// DUMP파일을 로드하여 재개
void machine_LoadMachine(std::string path)
{
	//FILE* fp = fopen("appleIIdump.dmp", "rb");
	FILE* fp = fopen(path.c_str(), "rb");
	if (fp != NULL)
	{
		mem.LoadDump(fp);
		cpu.LoadDump(fp);
		device.LoadDump(fp);
		fclose(fp);
	}
}

// 현재의 모든 상태를 저장
void machine_DumpMachine(std::string path)
{
	path += ".dmp";

	FILE* fp = fopen(path.c_str(), "wb");

	mem.Dump(fp);
	cpu.Dump(fp);
	device.Dump(fp);

	fclose(fp);
}

*/