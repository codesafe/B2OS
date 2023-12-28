/*
    vga.h - VGA class header to communicate in text or graphics mode

    References:
    - https://files.osdev.org/mirrors/geezer/osd/graphics/modes.c
*/

#ifndef VGA_H
#define VGA_H

#include "types.h"
//#include "common/common.h"
//#include "kernel/port.h"

// Number of registers
#define	VGA_NUM_SEQ_REGS	5
#define	VGA_NUM_CRTC_REGS	25
#define	VGA_NUM_GC_REGS		9
#define	VGA_NUM_AC_REGS		21

class VGA 
{
protected:
/*
    // VGA Ports
    kernel::Port8Bit VGA_AC_INDEX;
    kernel::Port8Bit VGA_AC_WRITE;
    kernel::Port8Bit VGA_AC_READ;
    kernel::Port8Bit VGA_MISC_READ;
    kernel::Port8Bit VGA_MISC_WRITE;
    kernel::Port8Bit VGA_SEQ_INDEX;
    kernel::Port8Bit VGA_SEQ_DATA;
    kernel::Port8Bit VGA_GC_INDEX;
    kernel::Port8Bit VGA_GC_DATA;
    kernel::Port8Bit VGA_CRTC_INDEX;
    kernel::Port8Bit VGA_CRTC_DATA;
    kernel::Port8Bit VGA_INSTAT_READ;
*/
    // Graphics mode resolution settings
    uint32_t VGA_GRAPHICS_MODE_WIDTH;
    uint32_t VGA_GRAPHICS_MODE_HEIGHT;
    uint32_t VGA_GRAPHICS_MODE_COLOURDEPTH;
    uint8_t *frameBufferSegment;

    /* VGA colour constants. */
    // TODO: increase this to all 256 colours
    enum vga_colour {
        VGA_COLOUR_BLACK = 0,
        VGA_COLOUR_BLUE = 1,
        VGA_COLOUR_GREEN = 2,
        VGA_COLOUR_CYAN = 3,
        VGA_COLOUR_RED = 4,
        VGA_COLOUR_MAGENTA = 5,
        VGA_COLOUR_BROWN = 6,
        VGA_COLOUR_LIGHT_GREY = 7,
        VGA_COLOUR_DARK_GREY = 8,
        VGA_COLOUR_LIGHT_BLUE = 9,
        VGA_COLOUR_LIGHT_GREEN = 10,
        VGA_COLOUR_LIGHT_CYAN = 11,
        VGA_COLOUR_LIGHT_RED = 12,
        VGA_COLOUR_LIGHT_MAGENTA = 13,
        VGA_COLOUR_LIGHT_BROWN = 14,
        VGA_COLOUR_WHITE = 15,
    };

    // Graphics mode methods
    void writeRegisters(uint8_t* registers);
    uint8_t* getFrameBufferSegment();
    virtual uint8_t getColorIndex(uint8_t r, uint8_t g, uint8_t b);

    // Text mode methods
    static inline uint8_t vga_entry_colour(enum vga_colour fg, enum vga_colour bg) 
    {
        return fg | bg << 4;
    }
    
    static inline uint16_t vga_entry(unsigned char uc, uint8_t colour) 
    {
        return (uint16_t) uc | (uint16_t) colour << 8;
    }

    void terminal_setcolour(uint8_t colour);
    void terminal_putentryat(char c, uint8_t colour, size_t x, size_t y);
    void terminal_putchar(char c);

public:
    // Text mode variables
    // Defined as static as they are global in scope
    static const size_t VGA_TEXT_MODE_WIDTH = 80;
    static const size_t VGA_TEXT_MODE_HEIGHT = 25;
    static size_t terminal_row;
    static size_t terminal_column;
    static uint8_t terminal_colour;
    static uint16_t *terminal_buffer;
    static bool isWelcome; // For checking if it is a welcome message, stops printing "$ "  

    // Public methods
    VGA(); // Constructor
    ~VGA(); // Destructor

    // Text mode methods
    void terminal_initialize(void);
    void terminal_write(const char* data, size_t size);
    void print_welcome_msg();
    size_t strlen(const char* str);
    void kprintf(const char* data);

    // Graphics mode methods
    virtual bool setMode(uint32_t width, uint32_t height, uint32_t colourDepth);
    virtual bool supportsMode(uint32_t width, uint32_t height, uint32_t colourDepth);    
    virtual void putPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b);
    virtual void putPixel(int32_t x, int32_t y, uint8_t colourIndex);
    virtual void putLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint8_t r, uint8_t g, uint8_t b);
    virtual void putRect(int32_t x0, int32_t y0, int32_t w, int32_t h, uint8_t r, uint8_t g, uint8_t b);
};

#endif // VGA_H