
#include "memory.h"
#include "low_level.h"
#include "vga.h"
#include "console.h"

#if 1

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

	for(int i=0;i<SCREEN_PAGE_NUM; i++)
		screenbuffer[i] = (unsigned char *)malloc(width * height * 3);

	return true;
}


void k_clearVga(unsigned int color)
{
	k_drawRect(0, 0, vgainfo->width, vgainfo->height, color);
}

void k_drawPixel(unsigned int  x, unsigned int  y, unsigned int color)
{

}

void k_drawRect(unsigned int xpos, unsigned int ypos, unsigned int width, unsigned int height, unsigned int color)
{
    for(int y = 0; y < height*2; y++)
    {
        int offset = (vgainfo->width * 3 * (ypos + y)) + (xpos * 3);
        for(int x = 0; x < width*3; x+=3)
        {
            screenbuffer[currentScreenpage][offset+x+0] = (color & 0xFF); 			//B
            screenbuffer[currentScreenpage][offset+x+1] = (color & 0xFF00) >> 8; 	//G
            screenbuffer[currentScreenpage][offset+x+2] = (color & 0xFF0000) >> 16;	//R
        }
    }
}

void k_swapBuffer()
{
    unsigned char* frame = (unsigned char*)vgainfo->frameBuffer;// - 0xC0000000;
    for(int i = 0; i < (vgainfo->width * vgainfo->height * 3); i++)
    {
        frame[i] = screenbuffer[currentScreenpage][i];
    }

	currentScreenpage = currentScreenpage==0?1:0;
}

#else

unsigned char *frameBufferSegment = 0;
int screenX = 0;
int screenY = 0;

void set_appleii_palette()
{
	// apple2 palette 적용
    unsigned char palette_RGB[64][3] = 
	{
		{ 0,   0,   0   }, { 0, 255, 0 }, 
		{ 126, 110, 173 }, { 255, 255, 255 },
		{ 0,   0,   0   }, { 234, 108, 21  }, 
		{ 86,  168, 228 }, { 255, 255, 255 },
		{ 0,   0,   0   }, { 63,  55, 86  }, 
		{ 72,  96,  25	}, { 255, 255, 255 },
		{ 0,   0,   0   }, { 43,  84, 114 }, 
		{ 117, 54,  10 }, { 255, 255, 255 },

		{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
    };

    // /* Start writing at color 0. */
    // OUTB (VGA_DAC_CONTROL, 0x00);
    // /* Write all 64 colors from array. */
    // REP_OUTSB (VGA_DAC_PALETTE, palette_RGB, 64 * 3);

    OUTB(0x03C6,0xff);
    OUTB(0x3c8,0);
    for(unsigned int c=0;c<64;c++)
    {  
        OUTB(0x3c9, palette_RGB[c][0]);
        OUTB(0x3c9, palette_RGB[c][1]);
        OUTB(0x3c9, palette_RGB[c][2]);
    }

}

void set_mode_x_palette()
{
    /* 6-bit RGB (red, green, blue) values for first 64 colors */
    /* these are coded for 2 bits red, 2 bits green, 2 bits blue */
    unsigned char palette_RGB[64][3] = 
	{
		{0x00, 0x00, 0x00}, {0x00, 0x00, 0x15},
		{0x00, 0x00, 0x2A}, {0x00, 0x00, 0x3F},
		{0x00, 0x15, 0x00}, {0x00, 0x15, 0x15},
		{0x00, 0x15, 0x2A}, {0x00, 0x15, 0x3F},
		{0x00, 0x2A, 0x00}, {0x00, 0x2A, 0x15},
		{0x00, 0x2A, 0x2A}, {0x00, 0x2A, 0x3F},
		{0x00, 0x3F, 0x00}, {0x00, 0x3F, 0x15},
		{0x00, 0x3F, 0x2A}, {0x00, 0x3F, 0x3F},
		{0x15, 0x00, 0x00}, {0x15, 0x00, 0x15},
		{0x15, 0x00, 0x2A}, {0x15, 0x00, 0x3F},
		{0x15, 0x15, 0x00}, {0x15, 0x15, 0x15},
		{0x15, 0x15, 0x2A}, {0x15, 0x15, 0x3F},
		{0x15, 0x2A, 0x00}, {0x15, 0x2A, 0x15},
		{0x15, 0x2A, 0x2A}, {0x15, 0x2A, 0x3F},
		{0x15, 0x3F, 0x00}, {0x15, 0x3F, 0x15},
		{0x15, 0x3F, 0x2A}, {0x15, 0x3F, 0x3F},
		{0x2A, 0x00, 0x00}, {0x2A, 0x00, 0x15},
		{0x2A, 0x00, 0x2A}, {0x2A, 0x00, 0x3F},
		{0x2A, 0x15, 0x00}, {0x2A, 0x15, 0x15},
		{0x2A, 0x15, 0x2A}, {0x2A, 0x15, 0x3F},
		{0x2A, 0x2A, 0x00}, {0x2A, 0x2A, 0x15},
		{0x2A, 0x2A, 0x2A}, {0x2A, 0x2A, 0x3F},
		{0x2A, 0x3F, 0x00}, {0x2A, 0x3F, 0x15},
		{0x2A, 0x3F, 0x2A}, {0x2A, 0x3F, 0x3F},
		{0x3F, 0x00, 0x00}, {0x3F, 0x00, 0x15},
		{0x3F, 0x00, 0x2A}, {0x3F, 0x00, 0x3F},
		{0x3F, 0x15, 0x00}, {0x3F, 0x15, 0x15},
		{0x3F, 0x15, 0x2A}, {0x3F, 0x15, 0x3F},
		{0x3F, 0x2A, 0x00}, {0x3F, 0x2A, 0x15},
		{0x3F, 0x2A, 0x2A}, {0x3F, 0x2A, 0x3F},
		{0x3F, 0x3F, 0x00}, {0x3F, 0x3F, 0x15},
		{0x3F, 0x3F, 0x2A}, {0x3F, 0x3F, 0x3F}
    };

    /* Start writing at color 0. */
    OUTB (VGA_DAC_CONTROL, 0x00);
    /* Write all 64 colors from array. */
    REP_OUTSB (VGA_DAC_PALETTE, palette_RGB, 64 * 3);
}

bool k_setVgaMode(int width, int height, int colourDepth)
{
	screenX = width;
	screenY = height;
	//VGA_GRAPHICS_MODE_COLOURDEPTH = colourDepth;
	
	// register sets
	unsigned char _320x200x256[] =	
    {
	/* MISC */
		0x63,
	/* SEQ */
		0x03, 0x01, 0x0F, 0x00, 0x0E,
	/* CRTC */
		0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
		0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x9C, 0x0E, 0x8F, 0x28,	0x40, 0x96, 0xB9, 0xA3,
		0xFF,
	/* GC */
		0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
		0xFF,
	/* AC */
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
		0x41, 0x00, 0x0F, 0x00,	0x00
	};

	///////////////////////////////////////////////////////////////

	unsigned char _320x200x256_modex[] =
	{
	/* MISC */
		0x63,
	/* SEQ */
		0x03, 0x01, 0x0F, 0x00, 0x06,
	/* CRTC */
		0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
		0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x9C, 0x0E, 0x8F, 0x28, 0x00, 0x96, 0xB9, 0xE3,
		0xFF,
	/* GC */
		0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
		0xFF,
	/* AC */
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
		0x41, 0x00, 0x0F, 0x00, 0x00
	};


	writeRegisters(_320x200x256);
	//writeRegisters(_320x200x256_modex);
	
	//set_mode_x_palette();
	set_appleii_palette();

	frameBufferSegment = getFrameBufferSegment();
	return true;
}


// VGA framebuffer is at A000:0000, B000:0000, or B800:0000
// depending on bits in GC 6
unsigned char* getFrameBufferSegment() 
{
    port_byte_out( VGA_GC_INDEX, 0x06);

#if false
	//uint8_t segment = VGA_GC_DATA.read() & (3<<2);
    unsigned char segment = port_byte_in(VGA_GC_DATA) & (3<<2);
	switch(segment) 
    {
        default:
        case 0<<2: return (unsigned char*)0x00000;
        case 1<<2: return (unsigned char*)0xA0000;
        case 2<<2: return (unsigned char*)0xB0000;
        case 3<<2: return (unsigned char*)0xB8000;
    }
#else
	unsigned char segment = port_byte_in(VGA_GC_DATA);
	unsigned char *ret = 0; 
	segment >>= 2;
	segment &= 3;

	switch(segment)
	{
	case 0:
	case 1:
		ret = (unsigned char*)0xA0000;
		break;
	case 2:
		ret = (unsigned char*)0xB0000;
		break;
	case 3:
		ret = (unsigned char*)0xB8000;
		break;
	}
	return ret;
#endif	
}

void writeRegisters(unsigned char* regs) 
{
	// Write Misc registers
    port_byte_out( VGA_MISC_WRITE, *(regs++) );
	//VGA_MISC_WRITE.write(*(regs++));

	// Write Sequencer registers
	for (unsigned char i = 0; i < VGA_NUM_SEQ_REGS; i++) 
    {
		//VGA_SEQ_INDEX.write(i);
        port_byte_out( VGA_SEQ_INDEX, i );

		//VGA_SEQ_DATA.write(*(regs++));
        port_byte_out( VGA_SEQ_DATA, *(regs++));
	}

	// Unlock CRTC registers
	//VGA_CRTC_INDEX.write(0x03);
    port_byte_out( VGA_CRTC_INDEX, 0x03);
    //VGA_CRTC_DATA.write(VGA_CRTC_DATA.read() | 0x80);
    port_byte_out( VGA_CRTC_DATA, port_byte_in(VGA_CRTC_DATA) | 0x80);
	//VGA_CRTC_INDEX.write(0x11);
    port_byte_out( VGA_CRTC_INDEX, 0x11);
	//VGA_CRTC_DATA.write(VGA_CRTC_DATA.read() & ~0x80);
    port_byte_out( VGA_CRTC_DATA, port_byte_in(VGA_CRTC_DATA) & ~0x80);

	// Make sure they remain unlocked
	regs[0x03] |= 0x80;
	regs[0x11] &= ~0x80;

	// Write CRTC registers
	for(unsigned char i = 0; i < VGA_NUM_CRTC_REGS; i++) 
    {
		//VGA_CRTC_INDEX.write(i);
        port_byte_out( VGA_CRTC_INDEX, i);
		//VGA_CRTC_DATA.write(*(regs++));
        port_byte_out( VGA_CRTC_DATA, *(regs++));
	}

	// Write Graphics Controller registers
	for(unsigned char i = 0; i < VGA_NUM_GC_REGS; i++) 
    {
		//VGA_GC_INDEX.write(i);
        port_byte_out( VGA_GC_INDEX, i);
		//VGA_GC_DATA.write(*(regs++));
        port_byte_out( VGA_GC_DATA, *(regs++));
	}

	// Write Attribute Controller registers
	for(unsigned char i = 0; i < VGA_NUM_AC_REGS; i++) 
    {
		//VGA_INSTAT_READ.read();
        port_byte_in( VGA_INSTAT_READ );
		//VGA_AC_INDEX.write(i);
        port_byte_out( VGA_AC_INDEX, i);
		//VGA_AC_WRITE.write(*(regs++));
        port_byte_out( VGA_AC_WRITE, *(regs++));
	}

	// Lock 16-colour palette and unblank display
	//VGA_INSTAT_READ.read();
    port_byte_in( VGA_INSTAT_READ );
	//VGA_AC_INDEX.write(0x20);
    //port_byte_out( VGA_AC_INDEX, 0x20);
	port_byte_out( VGA_AC_INDEX, 0xFF);
}

void k_clearVga(unsigned char color)
{
	for(int y=0; y<screenY; y++)
		for(int x=0; x<screenX; x++)
		{
			k_drawPixel(x, y, color);
		}
}

void k_drawPixel(int x, int y, unsigned char colourIndex) 
{
	if(x < 0 || screenX <= x || y < 0 || screenY <= y)
        return;
	unsigned char *pixelAddr = frameBufferSegment + (screenX*y + x);
	*pixelAddr = colourIndex;
}


#endif