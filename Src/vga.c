
#include "memory.h"
#include "low_level.h"
#include "vga.h"
#include "vbe.h"
#include "console.h"

#include "vbe.h"
#define SCREEN_PAGE_NUM 2

struct Video_Info *vgainfo;
unsigned char *screenbuffer[SCREEN_PAGE_NUM];
unsigned char currentScreenpage = 0;

bool k_setVgaMode(int width, int height, int bpp)
{
	vgainfo = vbe_init(width, height, bpp);
	// k_printnum(info->width);
	// k_printnum(info->height);
	// k_printnum(info->bpp);

	// for(int i=0;i<SCREEN_PAGE_NUM; i++)
    // {
    //     screenbuffer[i] = (unsigned char *)malloc(width * height * 3);

    //     unsigned char *screen = screenbuffer[i];
    //     int color = 0xFF00FFFF;
    //     for(int y=0; y<height; y++)
    //         for(int x=0; x<width;x++)
    //         {
    //             uint32 pixel_offset = y*width + (x * 3);
    //             screen[pixel_offset+0] = (color & 0xFF); 			//B
    //             screen[pixel_offset+1] = (color & 0xFF00) >> 8; 	//G
    //             screen[pixel_offset+2] = (color & 0xFF0000) >> 16;	//R
    //         }

    // }
		


	return true;
}


void k_clearVga(unsigned int color)
{
    unsigned char* frame = (unsigned char*)vgainfo->frameBuffer;
    for(int i = 0; i < vgainfo->width * vgainfo->height; i++)
    {
        int offset = i*3;
        frame[offset+0] = (color & 0xFF); 			//B
        frame[offset+1] = (color & 0xFF00) >> 8; 	//G
        frame[offset+2] = (color & 0xFF0000) >> 16;	//R
    }

	//k_drawRect(0, 0, vgainfo->width, vgainfo->height, color);
}

void k_drawPixel(unsigned int  x, unsigned int  y, unsigned int color)
{
    return;
	unsigned char* frame = (unsigned char*)vgainfo->frameBuffer;
    int offset = (vgainfo->width * y * 3) + (x * 3);
    frame[offset+0] = (color & 0xFF); 			//B
    frame[offset+1] = (color & 0xFF00) >> 8; 	//G
    frame[offset+2] = (color & 0xFF0000) >> 16;	//R
}

void k_drawRect(unsigned int xpos, unsigned int ypos, unsigned int width, unsigned int height, unsigned int color)
{
	unsigned char* frame = (unsigned char*)vgainfo->frameBuffer;

    for(int y = 0; y < height*2; y++)
    {
        int offset = (vgainfo->width * 3 * (ypos + y)) + (xpos * 3);
        for(int x = 0; x < width*3; x+=3)
        {
            // screenbuffer[currentScreenpage][offset+x+0] = (color & 0xFF); 			//B
            // screenbuffer[currentScreenpage][offset+x+1] = (color & 0xFF00) >> 8; 	//G
            // screenbuffer[currentScreenpage][offset+x+2] = (color & 0xFF0000) >> 16;	//R
            frame[offset+x+0] = (color & 0xFF); 			//B
            frame[offset+x+1] = (color & 0xFF00) >> 8; 	//G
            frame[offset+x+2] = (color & 0xFF0000) >> 16;	//R


        }
    }
}

void k_swapBuffer()
{
    unsigned char* frame = (unsigned char*)vgainfo->frameBuffer;
    for(int i = 0; i < (vgainfo->width * vgainfo->height * 3); i++)
    {
        frame[i] = screenbuffer[currentScreenpage][i];
    }
	currentScreenpage = currentScreenpage==0?1:0;
}
