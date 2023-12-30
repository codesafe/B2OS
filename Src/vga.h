#ifndef VGA_H
#define VGA_H

#include "../Driver/types.h"

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

extern unsigned char *frameBufferSegment;

bool setMode(uint32_t width, uint32_t height, uint32_t colourDepth);
void writeRegisters(uint8_t* regs);
uint8_t* getFrameBufferSegment();;

#endif