#include "vga.h"
//using namespace morphios::common;
//using namespace morphios::drivers;
//using namespace morphios::kernel;

void VGA_Init()
{
/*
	VGA_AC_INDEX(0x3C0),
	VGA_AC_WRITE(0x3C0),
	VGA_AC_READ(0x3C1),
	VGA_MISC_READ(0x3CC),
	VGA_MISC_WRITE(0x3C2),
	VGA_SEQ_INDEX(0x3C4),
	VGA_SEQ_DATA(0x3C5),
	VGA_GC_INDEX(0x3CE),
	VGA_GC_DATA(0x3CF),
	VGA_CRTC_INDEX(0x3D4),
	VGA_CRTC_DATA(0x3D5),
	VGA_INSTAT_READ(0x3DA)
*/	
}

// Text mode methods

size_t terminal_row = 0;
size_t terminal_column = 0;
uint8_t terminal_colour = 0;
uint16_t* terminal_buffer = 0;
bool isWelcome = true;

void terminal_initialize(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_colour = vga_entry_colour(VGA_COLOUR_GREEN, VGA_COLOUR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_TEXT_MODE_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_TEXT_MODE_WIDTH; x++) {
			const size_t index = y * VGA_TEXT_MODE_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_colour);
		}
	}
}

void terminal_setcolour(uint8_t colour) 
{
	terminal_colour = colour;
}
 
void terminal_putentryat(char c, uint8_t colour, size_t x, size_t y) {
	const size_t index = y * VGA_TEXT_MODE_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, colour);
}

void terminal_putchar(char c) 
{
	if (c == '\n') 
    {
		terminal_column = 0;
		terminal_row++;
		if (terminal_row == VGA_TEXT_MODE_HEIGHT)
			terminal_initialize();
		if(!isWelcome) 
        {
			terminal_write("$ ", strlen("$ "));
			terminal_column = 2;
		}
	}
	else if (c == '\0') {}
	else if (c == '\b') {
		if (terminal_column > 2) {
			terminal_column--;
			terminal_putentryat('\0', terminal_colour, terminal_column, terminal_row);
		}
		else {
			terminal_column = 2;
		}
	}
	else {
		terminal_putentryat(c, terminal_colour, terminal_column, terminal_row);
		if (++terminal_column == VGA_TEXT_MODE_WIDTH) {
			terminal_column = 0;
			if (++terminal_row == VGA_TEXT_MODE_HEIGHT)
				terminal_initialize();
		}
	}	
}
 
void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void print_welcome_msg() 
{
	kprintf("                               __    _ ____  _____\n");
	kprintf("   ____ ___  ____  _________  / /_  (_) __ \\/ ___/\n");
	kprintf("  / __ `__ \\/ __ \\/ ___/ __ \\/ __ \\/ / / / /\\__ \\ \n");
	kprintf(" / / / / / / /_/ / /  / /_/ / / / / / /_/ /___/ / \n");
	kprintf("/_/ /_/ /_/\\____/_/  / .___/_/ /_/_/\\____//____/  \n");
	kprintf("                    /_/                           \n");
	kprintf("\nWake up, Neo\n");
	kprintf("The Matrix has you\nFollow the white rabbit\n...\nKnock, Knock, Neo.\n");	
}

// Graphics mode methods

bool setMode(uint32_t width, uint32_t height, uint32_t colourDepth)
{
	if (!supportsMode(width, height, colourDepth))
		return false;
	
	VGA_GRAPHICS_MODE_WIDTH = width;
	VGA_GRAPHICS_MODE_HEIGHT = height;
	VGA_GRAPHICS_MODE_COLOURDEPTH = colourDepth;
	
	// register sets
	unsigned char g_320x200x256[] =	{
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
	writeRegisters(g_320x200x256);

	frameBufferSegment = getFrameBufferSegment();

	return true;
}

// TODO: add support for other display modes
bool supportsMode(uint32_t width, uint32_t height, uint32_t colourDepth) 
{
	if ((width == 320) && (height == 200) && (colourDepth == 8))
		return true;
	return false;
}

void writeRegisters(uint8_t* regs) 
{
#if 0
	// Write Misc registers
	VGA_MISC_WRITE.write(*(regs++));

	// Write Sequencer registers
	for (uint8_t i = 0; i < VGA_NUM_SEQ_REGS; i++) {
		VGA_SEQ_INDEX.write(i);
		VGA_SEQ_DATA.write(*(regs++));
	}

	// Unlock CRTC registers
	VGA_CRTC_INDEX.write(0x03);
	VGA_CRTC_DATA.write(VGA_CRTC_DATA.read() | 0x80);
	VGA_CRTC_INDEX.write(0x11);
	VGA_CRTC_DATA.write(VGA_CRTC_DATA.read() & ~0x80);

	// Make sure they remain unlocked
	regs[0x03] |= 0x80;
	regs[0x11] &= ~0x80;

	// Write CRTC registers
	for(uint8_t i = 0; i < VGA_NUM_CRTC_REGS; i++) {
		VGA_CRTC_INDEX.write(i);
		VGA_CRTC_DATA.write(*(regs++));
	}

	// Write Graphics Controller registers
	for(uint8_t i = 0; i < VGA_NUM_GC_REGS; i++) {
		VGA_GC_INDEX.write(i);
		VGA_GC_DATA.write(*(regs++));
	}

	// Write Attribute Controller registers
	for(uint8_t i = 0; i < VGA_NUM_AC_REGS; i++) {
		VGA_INSTAT_READ.read();
		VGA_AC_INDEX.write(i);
		VGA_AC_WRITE.write(*(regs++));
	}

	// Lock 16-colour palette and unblank display
	VGA_INSTAT_READ.read();
	VGA_AC_INDEX.write(0x20);
#endif    
}


void putPixel(int32_t x, int32_t y,  uint8_t r, uint8_t g, uint8_t b) 
{
	putPixelIndex(x, y, getColorIndex(r, g, b));
}

void putPixelIndex(int32_t x, int32_t y, uint8_t colourIndex) 
{
	// Limit pixel writing to within the screen width
	if(x < 0 || VGA_GRAPHICS_MODE_WIDTH <= x || y < 0 || VGA_GRAPHICS_MODE_HEIGHT <= y)
        return;

	uint8_t *pixelAddr = frameBufferSegment + VGA_GRAPHICS_MODE_WIDTH*y + x;
	*pixelAddr = colourIndex;
}

// Converts RGB colours to the VGA colour
// TODO: add all 256 VGA colours
uint8_t getColorIndex(uint8_t r, uint8_t g, uint8_t b) 
{
	if(r == 0x00 && g == 0x00 && b == 0x00) return VGA_COLOUR_BLACK;
    if(r == 0x00 && g == 0x00 && b == 0xA8) return VGA_COLOUR_BLUE;
    if(r == 0x00 && g == 0xA8 && b == 0x00) return VGA_COLOUR_GREEN;
    if(r == 0xA8 && g == 0x00 && b == 0x00) return VGA_COLOUR_RED;
    if(r == 0xFF && g == 0xFF && b == 0xFF) return VGA_COLOUR_WHITE;
    return VGA_COLOUR_BLACK; // default
}

// VGA framebuffer is at A000:0000, B000:0000, or B800:0000
// depending on bits in GC 6
uint8_t* getFrameBufferSegment() 
{
#if 0    
	VGA_GC_INDEX.write(0x06);
	uint8_t segment = VGA_GC_DATA.read() & (3<<2);
	switch(segment) {
        default:
        case 0<<2: return (uint8_t*)0x00000;
        case 1<<2: return (uint8_t*)0xA0000;
        case 2<<2: return (uint8_t*)0xB0000;
        case 3<<2: return (uint8_t*)0xB8000;
    }
#endif
    return (uint8_t*)0xB8000;
}

void putLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1,  uint8_t r, uint8_t g, uint8_t b) 
{
	
	// Using Bresenham's line algorithm
	int32_t _dx, _dy, _D, _x, _y;

	_dx = x1 - x0;
	_dy = y1 - y0;
	_x = x0;
	_y = y0;
	_D = 2*_dy - _dx;

	while(_x < x1) {
		putPixel(_x, _y, r, g, b);
		if (_D > 0) {
			_y += 1;
			_D += 2*_dy - 2*_dx;
		}
		else {
			_D += 2*_dy;
		}
		_x += 1;
	}
}
void putRect(int32_t x0, int32_t y0, int32_t w, int32_t h, uint8_t r, uint8_t g, uint8_t b) 
{
	for(int32_t y = y0; y < y0 + h; y++) {
		for(int32_t x = x0; x < x0 + w; x++) {
			putPixel(x, y, r, g, b);
		}
	}
}

size_t  strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

void  kprintf(const char* data) 
{
	terminal_write(data, strlen(data));
}
