#include "vbe.h"
//#include "../io/serial.h"
#include "console.h"

typedef struct __attribute__ ((packed)) 
{
    unsigned short di, si, bp, sp, bx, dx, cx, ax;
    unsigned short gs, fs, es, ds, eflags;
} regs16_t;

extern void int32(unsigned char intnum, regs16_t *regs);

struct vbe_info_structure 
{
   char signature[4];   // "VESA"
   short version;
   int oem;
   int capabilities;
   int video_modes;
   char video_memory;
   char software_rev;
   int vendor;
   int product_name;
   int product_rev;
   char reserved[222];
   char oem_data[256];
} __attribute__ ((packed));

struct vbe_mode_info_structure 
{
   short attributes;
   char window_a;
   char window_b;
   short granularity;
   short window_size;
   short segment_a;
   short segment_b;
   int win_func_ptr;
   short pitch;         // number of bytes per horizontal line
   short width;         // width in pixels
   short height;         // height in pixels
   char w_char;
   char y_char;
   char planes;
   char bpp;         // bits per pixel in this mode
   char banks;
   char memory_model;
   char bank_size;
   char image_pages;
   char reserved0;

   char red_mask;
   char red_position;
   char green_mask;
   char green_position;
   char blue_mask;
   char blue_position;
   char reserved_mask;
   char reserved_position;
   char direct_color_attributes;

   int framebuffer;      // physical address of the linear frame buffer; write here to draw to the screen
   int off_screen_mem_off;
   short off_screen_mem_size;   // size of memory in the framebuffer but not being displayed on the screen
   char reserved1[206];
} __attribute__ ((packed));

struct vbe_mode_info_structure *vbeModeInfo = (struct vbe_mode_info_structure*)0xEE00;
struct vbe_info_structure *vbeInfo = (struct vbe_info_structure*)0xF000;

unsigned int* frameBuffer;
unsigned char* kenel_frameBuffer;

int vbe_width;
int vbe_height;
struct Video_Info info;

// https://wiki.osdev.org/User:Omarrx024/VESA_Tutorial
struct Video_Info *vbe_init(int x, int y, int bpp)
{
    vbeInfo->signature[0] = 'V';
    vbeInfo->signature[1] = 'E';
    vbeInfo->signature[2] = 'S';
    vbeInfo->signature[3] = 'A';

/*
FUNCTION: Get VESA BIOS information
Function code: 0x4F00
*/
    regs16_t regs;
    regs.ax = 0x4f00;
    regs.di = 0xF000;
    int32(0x10, &regs);

/*
FUNCTION: Get VESA mode information
Function code: 0x4F01
*/
    regs16_t regs2;
    regs2.ax = 0x4F01;
    regs2.cx = 0x112;
    regs2.di = 0xEE00;
    int32(0x10, &regs2);


/*
    MODE    RESOLUTION  BITS PER PIXEL  MAXIMUM COLORS
    0x0100  640x400     8               256
    0x0101  640x480     8               256
    0x0102  800x600     4               16
    0x0103  800x600     8               256
    0x010D  320x200     15              32k
    0x010E  320x200     16              64k
    0x010F  320x200     24/32*          16m
    0x0110  640x480     15              32k
    0x0111  640x480     16              64k
    0x0112  640x480     24/32*          16m
    0x0113  800x600     15              32k
    0x0114  800x600     16              64k
    0x0115  800x600     24/32*          16m
    0x0116  1024x768    15              32k
    0x0117  1024x768    16              64k
    0x0118  1024x768    24/32*          16m
*/
/*
FUNCTION: Set VBE mode
Function code: 0x4F02
*/
    unsigned short mode = 0x4112;
    if( x == 320 && y == 200 && bpp == 24 )
    {
        mode = 0x410F;
    }
    else if( x == 640 && y == 480 && bpp == 24 )
    {
        mode = 0x4112;
    }
    else if( x == 800 && y == 600 && bpp == 24 )
    {
        mode = 0x4115;
    }
    else if( x == 1024 && y == 600 && 768 == 24 )
    {
        mode = 0x4118;
    }

    regs16_t regs3;
    regs3.ax = 0x4F02;
    regs3.bx = mode;  // 0 ~ 13 bit mode : 0x0112 (640x480 24/32*)
    int32(0x10, &regs3);

    // printf("W:%d\n",vbeModeInfo->width);
    // printf("H:%d\n",vbeModeInfo->height);
    // printf("C:%d\n",vbeModeInfo->bpp);
    // printf("B:%x\n",vbeModeInfo->framebuffer);

    frameBuffer = (unsigned int*)vbeModeInfo->framebuffer;
    kenel_frameBuffer = (unsigned char*)frameBuffer - 0xc0000000;
    vbe_width = vbeModeInfo->width;
    vbe_height = vbeModeInfo->height;

    asm("cli");

    info.frameBuffer = (unsigned char*)vbeModeInfo->framebuffer;
    info.kenel_frameBuffer = (unsigned char*)frameBuffer - 0xc0000000;
    info.width =  vbeModeInfo->width;
    info.height = vbeModeInfo->height;
    info.bpp = vbeModeInfo->bpp;
    info.pitch = vbeModeInfo->pitch;

    return &info;
}