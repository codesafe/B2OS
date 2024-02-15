#ifndef APPLE2_MACHINE_H
#define APPLE2_MACHINE_H

#include "AppleCpu.h"
#include "AppleMem.h"
#include "AppleDevice.h"


void machine_InitMachine();
bool machine_Booting();

void machine_Reset();
void machine_Run(unsigned long cycle);
void machine_Render(int frame);


#endif
