#ifndef APPLE2_DEVICE_H
#define APPLE2_DEVICE_H

//#include <stdio.h>
//#include <string>
#include "AppleFont.h"

// two disk ][ drive units
typedef struct FloppyDrive
{
	// the full disk image pathname
	char filename[400];
	// based on the image file attributes
	bool readOnly;
	// nibblelized disk image
	BYTE data[232960];
	// motor status
	bool motorOn;
	// writes to file are not implemented
	bool writeMode;
	// current track position
	BYTE track;
	// ptr to nibble under head position
	WORD nibble;
} FloppyDrive;

struct _RECT
{
	int x, y, width, height;
};

// typedef struct Color 
// {
//     unsigned char r;        // Color red value
//     unsigned char g;        // Color green value
//     unsigned char b;        // Color blue value
//     unsigned char a;        // Color alpha value
// } Color;

/*
struct GamePad
{
	SDL_Joystick* controller;
	bool isavailable;
	bool axis[4];
	bool pressbtn1;
	bool pressbtn2;

	GamePad()
	{
		controller = NULL;
		isavailable = false;
		axis[0] = axis[1] = axis[2] = axis[3] = false;
		pressbtn1 = false;
		pressbtn2 = false;
	}
};
*/


	// extern bool loadromfile;
	// extern bool resetMachine;
	// extern bool colorMonitor;
	// extern BYTE zoomscale;

	//////////////////////////////////////////////////////////////////////////

	// extern float GCP[2]; // GC Position ranging from 0 (left) to 255 right
	// extern float GCC[2]; // $C064 (GC0) and $C065 (GC1) Countdowns
	// extern int GCD[2];// GC0 and GC1 Directions (left/down or right/up)
	// extern int GCA[2];// GC0 and GC1 Action (push or release)
	// extern BYTE GCActionSpeed; // Game Controller speed at which it goes to the edges
	// extern BYTE GCReleaseSpeed;// Game Controller speed at which it returns to center
	// extern long long int GCCrigger; // $C070 the tick at which the GCs were reseted

	//GamePad gamepad;

	//////////////////////////////////////////////////////////////////////////

	extern int	currentDrive;

	//////////////////////////////////////////////////////////////////////////

	// extern bool textMode;
	// extern bool mixedMode;
	// extern bool hires_Mode;
	// extern BYTE videoPage;
	// extern WORD videoAddress;

	// extern struct _RECT pixelGR;

	// extern int LoResCache[24][40];
	// extern int HiResCache[192][40];
	// extern BYTE previousBit[192][40];
	// extern BYTE flashCycle;

	//extern struct Color* backbuffer;	// Render Backbuffer

	//extern struct AppleFont font;
	//BYTE keyboard;

	////////////////////////////////////////////////

//	extern struct FloppyDrive disk[2];
	// extern BYTE updatedrive;

	// extern bool phases[2][4];
	// // phases states Before
	// extern bool phasesB[2][4];
	// // phases states Before Before
	// extern bool phasesBB[2][4];
	// // phase index (for both drives)
	// extern int pIdx[2];
	// // phase index Before
	// extern int pIdxB[2];
	// extern int halfTrackPos[2];
	// extern BYTE dLatch;

	////////////////////////////////////////////////

	//short audioBuffer[2][AUDIOBUFFERSIZE];
	//unsigned int audioDevice;

	//bool silence;
	//int volume;
	//bool speaker;
	//long long int speakerLastTick;

	void ad_resetPaddles();
	BYTE ad_readPaddle(int pdl);

	// DISK2
	extern void	ad_ResetFloppy();
	extern bool ad_InsertFloppy(int drv);
	void ad_stepMotor(WORD address);
	void ad_setDrv(int drv);

	// Keyboard
	void ad_UpdateKeyBoard();
	// GamePad
	void ad_UpdateGamepad();

	void ad_ClearScreen();
	//void ad_DrawPoint(int x, int y, int r, int g, int b);
	//void ad_DrawRect(struct _RECT rect, int r, int g, int b);
	int ad_GetScreenMode();

	void ad_Create();
	void ad_Reset();

	extern BYTE ad_SoftSwitch(WORD address, BYTE value, bool WRT);
	void ad_PlaySound();
	void ad_Render(int frame);

	void ad_UpdateInput();

	extern bool ad_UpdateFloppyDisk();
	extern void ad_InsetFloppy();


#endif
