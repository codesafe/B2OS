#ifndef APPLE2_FONT_H
#define APPLE2_FONT_H

#include "Predef.h"


unsigned char* af_read_bmp(const char* fname, int* _w, int* _h);
unsigned char* af_read_bmp_memory(char* buffer, int* _w, int* _h);

void af_Create();
void af_RenderFont(unsigned char *backbuffer, int fontnum, int x, int y, bool inv);


#endif

