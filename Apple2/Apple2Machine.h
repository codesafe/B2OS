#ifndef APPLE2_MACHINE_H
#define APPLE2_MACHINE_H

//#include <stdio.h>
//#include "AppleCpu.h"
#include "AppleMem.h"
//#include "Apple2Device.h"


//extern CPU cpu;
extern Memory mem;
//extern Apple2Device device;


//void Apple2Machine();
//~Apple2Machine();

void machine_InitMachine();
bool machine_Booting();
bool machine_UploadRom();

void machine_Reset();
void machine_Run(int cycle);
void machine_Render(int frame);

	
	//void FileDroped(char* path);

	//void LoadMachine(std::string path);
	//void DumpMachine(std::string path);
};



#endif
