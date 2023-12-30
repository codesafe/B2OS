
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
    extern uint32_t VGA_GRAPHICS_MODE_WIDTH;
    extern uint32_t VGA_GRAPHICS_MODE_HEIGHT;
    extern uint32_t VGA_GRAPHICS_MODE_COLOURDEPTH;
    extern uint8_t *frameBufferSegment;

    #define VGA_COLOUR_BLACK 0
    #define VGA_COLOUR_BLUE  1
    #define VGA_COLOUR_GREEN  2
    #define VGA_COLOUR_CYAN  3
    #define VGA_COLOUR_RED  4
    #define VGA_COLOUR_MAGENTA 5
    #define VGA_COLOUR_BROWN  6
    #define VGA_COLOUR_LIGHT_GREY  7
    #define VGA_COLOUR_DARK_GREY  8
    #define VGA_COLOUR_LIGHT_BLUE  9
    #define VGA_COLOUR_LIGHT_GREEN  10
    #define VGA_COLOUR_LIGHT_CYAN  11
    #define VGA_COLOUR_LIGHT_RED  12
    #define VGA_COLOUR_LIGHT_MAGENTA  13
    #define VGA_COLOUR_LIGHT_BROWN  14
    #define VGA_COLOUR_WHITE 15

    void vga_init(void);
    // Graphics mode methods
    void writeRegisters(uint8_t* registers);
    uint8_t* getFrameBufferSegment();
    uint8_t getColorIndex(uint8_t r, uint8_t g, uint8_t b);

    // Text mode methods
    uint8_t vga_entry_colour(uint8_t fg, uint8_t bg);
    uint16_t vga_entry(unsigned char uc, uint8_t colour);

    void terminal_setcolour(uint8_t colour);
    void terminal_putentryat(char c, uint8_t colour, size_t x, size_t y);
    void terminal_putchar(char c);

    // Text mode variables
    // Defined as static as they are global in scope
    #define VGA_TEXT_MODE_WIDTH  80
    #define VGA_TEXT_MODE_HEIGHT 25
    extern size_t terminal_row;
    extern size_t terminal_column;
    extern uint8_t terminal_colour;
    extern uint16_t *terminal_buffer;
    extern bool isWelcome; // For checking if it is a welcome message, stops printing "$ "  

    // Text mode methods
    void terminal_initialize(void);
    void terminal_write(const char* data, size_t size);
    void print_welcome_msg();
    size_t strlen(const char* str);
    void kprintf(const char* data);

    // Graphics mode methods
    bool setMode(uint32_t width, uint32_t height, uint32_t colourDepth);
    bool supportsMode(uint32_t width, uint32_t height, uint32_t colourDepth);    
    void putPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b);
    void putPixelIndex(int32_t x, int32_t y, uint8_t colourIndex);
    void putLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint8_t r, uint8_t g, uint8_t b);
    void putRect(int32_t x0, int32_t y0, int32_t w, int32_t h, uint8_t r, uint8_t g, uint8_t b);


#endif