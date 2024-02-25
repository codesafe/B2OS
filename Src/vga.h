#ifndef VGA_H
#define VGA_H

#include "kerneldef.h"

extern int screenX;
extern int screenY;

bool k_setVgaMode(int width, int height, int bpp);
void k_clearVga(unsigned int color);
void k_drawPixel(unsigned int x, unsigned int y, unsigned int color);
void k_drawRect (unsigned int xpos, unsigned int ypos, unsigned int width, unsigned int height, unsigned int color);

void k_swapBuffer();


#endif