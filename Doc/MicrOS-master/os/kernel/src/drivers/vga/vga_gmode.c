#include "vga_gmode.h"
#include "assembly/io.h"
#include <string.h>
#include "vga.h"
#include "filesystems/filesystem.h"
#include "modes/mode_13h/mode_13h.h"
#include "../vbe/vbe.h"

#define peekb(S, O) *(unsigned char *)(16uL * (S) + (O))
#define pokeb(S, O, V) *(unsigned char *)(16uL * (S) + (O)) = (V)
#define pokew(S, O, V) *(unsigned short *)(16uL * (S) + (O)) = (V)
#define _vmemwr(DS, DO, S, N) memcpy((char *)((DS)*16 + (DO)), S, N)

#define bit_get(p,m) ((p) & (m))
#define bit_set(p,m) ((p) |= (m))
#define bit_clear(p,m) ((p) &= ~(m))
#define bit_flip(p,m) ((p) ^= (m))
#define bit_write(p,m,v) (v ? bit_set(p,m) : bit_clear(p,m))

static char mode = 3;



typedef struct _Os2BmpFileHeader
{
	uint16_t   FileType;      /* File type identifier */
	uint32_t  FileSize;      /* Size of the file in bytes */
	uint16_t   XHotSpot;      /* X coordinate of hotspot */
	uint16_t   YHotSpot;      /* Y coordinate of hotspot */
	uint32_t  BitmapOffset;  /* Starting position of image data in bytes */
} __attribute__((packed)) OS2BMPFILEHEADER;

typedef struct _Os21xBitmapHeader
{
	uint32_t Size;            /* Size of this header in bytes */
	uint32_t  Width;           /* Image width in pixels */
	uint32_t  Height;          /* Image height in pixels */
	uint16_t  NumPlanes;       /* Number of color planes */
	uint16_t  BitsPerPixel;    /* Number of bits per pixel */
} __attribute__((packed)) OS21XBITMAPHEADER;

typedef struct _Os21xPaletteElement
{
	uint8_t Blue;      /* Blue component */
	uint8_t Green;     /* Green component */
	uint8_t Red;       /* Red component */
} __attribute__((packed)) OS21XPALETTEELEMENT;

void drawMicrOSLogoIn13H()
{
	filesystem_file_info info;
	filesystem_get_file_info("A:/LOGO.BMP", &info);

	uint8_t *buffer = heap_kernel_alloc(info.size, 0);
	filesystem_read_file("A:/LOGO.BMP", buffer, 0, info.size);

	OS2BMPFILEHEADER fh;
	memcpy(&fh, buffer, sizeof(OS2BMPFILEHEADER));

	OS21XBITMAPHEADER bh;
	memcpy(&bh, buffer + sizeof(OS2BMPFILEHEADER), sizeof(OS21XBITMAPHEADER));

	for(int x = 0; x < bh.Width; x++)
		for(int y = 0; y < bh.Height; y++)
			mode13h_draw_pixel(buffer[fh.BitmapOffset + (bh.Height * bh.Width - bh.Width - y*bh.Width) + x],x,y);

	heap_kernel_dealloc(buffer);
}


void drawLenaIn13H()
{
	filesystem_file_info info;
	filesystem_get_file_info("A:/LENA.BMP", &info);

	uint8_t *buffer = heap_kernel_alloc(info.size, 0);
	filesystem_read_file("A:/LENA.BMP", buffer, 0, info.size);

	OS2BMPFILEHEADER fh;
	memcpy(&fh, buffer, sizeof(OS2BMPFILEHEADER));

	OS21XBITMAPHEADER bh;
	memcpy(&bh, buffer + sizeof(OS2BMPFILEHEADER), sizeof(OS21XBITMAPHEADER));

	for(int x = 0; x < bh.Width; x++)
		for(int y = 0; y < bh.Height; y++)
			mode13h_draw_pixel(buffer[fh.BitmapOffset + (bh.Height * bh.Width - bh.Width - y*bh.Width) + x],x,y);

	heap_kernel_dealloc(buffer);
}

void drawLenaIn10fH_linear(uint8_t* color)
{
	filesystem_file_info info;
	filesystem_get_file_info("A:/LENAD.BMP", &info);

	uint8_t *buffer = heap_kernel_alloc(info.size, 0);
	filesystem_read_file("A:/LENAD.BMP", buffer, 0, info.size);

	OS2BMPFILEHEADER fh;
	memcpy(&fh, buffer, sizeof(OS2BMPFILEHEADER));

	OS21XBITMAPHEADER bh;
	memcpy(&bh, buffer + sizeof(OS2BMPFILEHEADER), sizeof(OS21XBITMAPHEADER));

	for(int x = 0; x < bh.Width; x++)
		for(int y = 0; y < bh.Height; y++)
			{
				int i = 3*y * bh.Width + 3*x;
				color[i] = buffer[fh.BitmapOffset + (x * 3) + (bh.Height - y - 1) * 3 * bh.Width];
				color[i+1] = buffer[fh.BitmapOffset + (x * 3) + (bh.Height - y - 1) * 3 * bh.Width + 1];
				color[i+2] = buffer[fh.BitmapOffset + (x * 3) + (bh.Height - y - 1) * 3 * bh.Width + 2];
			}
	heap_kernel_dealloc(buffer);
}

void drawLenaIn10fH()
{
	filesystem_file_info info;
	filesystem_get_file_info("A:/LENAD.BMP", &info);

	uint8_t *buffer = heap_kernel_alloc(info.size, 0);
	filesystem_read_file("A:/LENAD.BMP", buffer, 0, info.size);

	OS2BMPFILEHEADER fh;
	memcpy(&fh, buffer, sizeof(OS2BMPFILEHEADER));

	OS21XBITMAPHEADER bh;
	memcpy(&bh, buffer + sizeof(OS2BMPFILEHEADER), sizeof(OS21XBITMAPHEADER));

	for(int x = 0; x < bh.Width; x++)
		for(int y = 0; y < bh.Height; y++)
			//mode13h_draw_pixel(buffer[fh.BitmapOffset + (bh.Height * bh.Width - bh.Width - y*bh.Width) + x],x,y);
			VBE_draw_pixel_8_8_8(320, 240, 64, 64, x, y, buffer[fh.BitmapOffset + (x * 3) + (bh.Height - y - 1) * 3 * bh.Width + 2], buffer[fh.BitmapOffset + (x * 3) + (bh.Height - y - 1) * 3 * bh.Width + 1], buffer[fh.BitmapOffset + (x * 3) + (bh.Height - y - 1) * 3 * bh.Width]);
	heap_kernel_dealloc(buffer);
}

char vga_gmode_get_mode()
{
	return mode;
}