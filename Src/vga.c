
//#include "../Driver/types.h"
#include "vga.h"

unsigned char *frameBufferSegment = 0;

bool setMode(uint32_t width, uint32_t height, uint32_t colourDepth)
{
	//VGA_GRAPHICS_MODE_WIDTH = width;
	//VGA_GRAPHICS_MODE_HEIGHT = height;
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

	writeRegisters(_320x200x256);
	frameBufferSegment = getFrameBufferSegment();

	return true;
}


// VGA framebuffer is at A000:0000, B000:0000, or B800:0000
// depending on bits in GC 6
uint8_t* getFrameBufferSegment() 
{
	//VGA_GC_INDEX.write(0x06);
    port_8_out( VGA_GC_INDEX, 0x06);

	//uint8_t segment = VGA_GC_DATA.read() & (3<<2);
    uint8_t segment = port_8_in(VGA_GC_DATA) & (3<<2);
	switch(segment) 
    {
        default:
        case 0<<2: return (uint8_t*)0x00000;
        case 1<<2: return (uint8_t*)0xA0000;
        case 2<<2: return (uint8_t*)0xB0000;
        case 3<<2: return (uint8_t*)0xB8000;
    }
}

void writeRegisters(uint8_t* regs) 
{
	// Write Misc registers
    port_8_out( VGA_MISC_WRITE, *(regs++) );
	//VGA_MISC_WRITE.write(*(regs++));

	// Write Sequencer registers
	for (uint8_t i = 0; i < VGA_NUM_SEQ_REGS; i++) 
    {
		//VGA_SEQ_INDEX.write(i);
        port_8_out( VGA_SEQ_INDEX, i );

		//VGA_SEQ_DATA.write(*(regs++));
        port_8_out( VGA_SEQ_DATA, *(regs++));
	}

	// Unlock CRTC registers
	//VGA_CRTC_INDEX.write(0x03);
    port_8_out( VGA_CRTC_INDEX, 0x03);
    //VGA_CRTC_DATA.write(VGA_CRTC_DATA.read() | 0x80);
    port_8_out( VGA_CRTC_DATA, port_8_in(VGA_CRTC_DATA) | 0x80);
	//VGA_CRTC_INDEX.write(0x11);
    port_8_out( VGA_CRTC_INDEX, 0x11);
	//VGA_CRTC_DATA.write(VGA_CRTC_DATA.read() & ~0x80);
    port_8_out( VGA_CRTC_DATA, port_8_in(VGA_CRTC_DATA) & ~0x80);

	// Make sure they remain unlocked
	regs[0x03] |= 0x80;
	regs[0x11] &= ~0x80;

	// Write CRTC registers
	for(uint8_t i = 0; i < VGA_NUM_CRTC_REGS; i++) 
    {
		//VGA_CRTC_INDEX.write(i);
        port_8_out( VGA_CRTC_INDEX, i);
		//VGA_CRTC_DATA.write(*(regs++));
        port_8_out( VGA_CRTC_DATA, *(regs++));
	}

	// Write Graphics Controller registers
	for(uint8_t i = 0; i < VGA_NUM_GC_REGS; i++) 
    {
		//VGA_GC_INDEX.write(i);
        port_8_out( VGA_GC_INDEX, i);
		//VGA_GC_DATA.write(*(regs++));
        port_8_out( VGA_GC_DATA, *(regs++));
	}

	// Write Attribute Controller registers
	for(uint8_t i = 0; i < VGA_NUM_AC_REGS; i++) 
    {
		//VGA_INSTAT_READ.read();
        port_8_in( VGA_INSTAT_READ );
		//VGA_AC_INDEX.write(i);
        port_8_out( VGA_AC_INDEX, i);
		//VGA_AC_WRITE.write(*(regs++));
        port_8_out( VGA_AC_WRITE, *(regs++));
	}

	// Lock 16-colour palette and unblank display
	//VGA_INSTAT_READ.read();
    port_8_in( VGA_INSTAT_READ );
	//VGA_AC_INDEX.write(0x20);
    port_8_out( VGA_AC_INDEX, 0x20);
}
