#ifndef VGA_H
#define VGA_H

#include "predef.h"


// Number of registers
#define	VGA_NUM_SEQ_REGS	5
#define	VGA_NUM_CRTC_REGS	25
#define	VGA_NUM_GC_REGS		9
#define	VGA_NUM_AC_REGS		21

#define VGA_AC_INDEX    0x3C0
#define VGA_AC_WRITE    0x3C0
#define VGA_AC_READ     0x3C1

#define VGA_MISC_READ   0x3CC
#define VGA_MISC_WRITE  0x3C2

#define VGA_SEQ_INDEX   0x3C4
#define VGA_SEQ_DATA    0x3C5

#define VGA_GC_INDEX    0x3CE
#define VGA_GC_DATA     0x3CF

#define VGA_CRTC_INDEX  0x3D4
#define VGA_CRTC_DATA   0x3D5

#define VGA_INSTAT_READ 0x3DA

// Port 0x3C8, 0x3C9 and 0x3C7 control the DAC.
#define VGA_DAC_CONTROL     0x03C8
#define VGA_DAC_PALETTE     0x03C9

extern unsigned char *frameBufferSegment;
extern int screenX;
extern int screenY;

bool setVgaMode(int width, int height, int colourDepth);
void clearVga(unsigned char color);
void writeRegisters(unsigned char* regs);
unsigned char* getFrameBufferSegment();;
void drawPixel(int x, int y, unsigned char colourIndex);

#endif