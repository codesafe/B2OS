//	#include "Predef.h"
#include "rombios.h"
#include "../Src/memory.h"
#include "AppleMachine.h"



void machine_InitMachine()
{
	mem_Create();
	ad_InsetFloppy();
	// unset the Power-UP byte
	mem_WriteByte(0x3F4, 0);
	// init cpu
	ac_Init();
	ac_Reset();

	// dirty hack, fix soon... if I understand why
	mem_WriteByte(0x4D, 0xAA);   // Joust crashes if this memory location equals zero
	mem_WriteByte(0xD0, 0xAA);   // Planetoids won't work if this memory location equals zero
	ad_Create();

	machine_Booting();
}

bool machine_Booting()
{
	memcpy(rom, appleIIrom, ROMSIZE);
	memcpy(sl6, diskII, SL6SIZE);
	ac_Reset();
	return true;
}

void machine_Reset()
{
	mem_Reset();
	ac_Reboot();
	ad_Reset();

	// unset the Power-UP byte
	mem_WriteByte(0x3F4, 0);
	// dirty hack, fix soon... if I understand why
	mem_WriteByte(0x4D, 0xAA);   // Joust crashes if this memory location equals zero
	mem_WriteByte(0xD0, 0xAA);   // Planetoids won't work if this memory location equals zero

	machine_Booting();
}

void machine_Run(int cycle)
{
	// if (resetMachine)
	// {
	// 	Reset();
	// 	return;
	// }

	ad_UpdateInput();
	ac_Run(cycle);
	while (1)
	{
		if( ad_UpdateFloppyDisk() == false ) 
			break;
		ac_Run(10000);
	}
}

void machine_Render(int frame)
{
	ad_Render(frame);
}

