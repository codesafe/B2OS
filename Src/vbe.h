#pragma once

#include <stddef.h>

typedef struct Video_Info 
{
    unsigned int width;
    unsigned int height;
    unsigned char bpp;
    unsigned short pitch;
    unsigned char* kenel_frameBuffer;
    unsigned char* frameBuffer;
} __attribute__ ((packed)) Video_Info;

struct Video_Info *vbe_init(int x, int y, int bpp);
extern unsigned int* frameBuffer;
extern unsigned char* kenel_frameBuffer;

extern int vbe_width;
extern int vbe_height;