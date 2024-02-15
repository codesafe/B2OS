
#include "../Src/memory.h"
#include "AppleFont.h"
//#include "Apple2Device.h"


unsigned char af_font[FONT_NUM][FONT_X*FONT_Y];
unsigned char af_invfont[FONT_NUM][FONT_X*FONT_Y];

const unsigned char normalfont[958] = {
	0x42, 0x4D, 0xBE, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00,
	0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0x08, 0x00,
	0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
	0xFF, 0x00, 0x6C, 0xD9, 0x0F, 0xF0, 0x1F, 0xD0, 0x07, 0x00, 0xA8, 0x40,
	0x80, 0x04, 0x3F, 0x7B, 0xDE, 0x04, 0x08, 0x0F, 0xE0, 0x2A, 0xAA, 0x00,
	0x00, 0x08, 0x0F, 0xE0, 0x3F, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0xC0,
	0x00, 0x00, 0x00, 0x00, 0x40, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x70, 0x00, 0x01, 0x00, 0x00, 0x00, 0x7E, 0xA5, 0xB2, 0xA2, 0x1B, 0xFF,
	0x08, 0x10, 0x00, 0xE0, 0x80, 0x0C, 0x3F, 0x04, 0x20, 0x24, 0x48, 0x08,
	0x02, 0x15, 0x55, 0xFF, 0xFC, 0x08, 0x80, 0x05, 0x00, 0xC0, 0x00, 0x20,
	0x01, 0x41, 0x01, 0x8D, 0x00, 0x10, 0x20, 0x40, 0x02, 0x00, 0x04, 0x00,
	0x38, 0x71, 0xF1, 0xC0, 0x87, 0x0E, 0x10, 0x38, 0xE0, 0x01, 0x00, 0x80,
	0x08, 0x08, 0x3C, 0x89, 0xE1, 0xC7, 0x8F, 0x90, 0x1E, 0x44, 0x70, 0xE2,
	0x27, 0xC8, 0x91, 0x1C, 0x40, 0x69, 0x11, 0xC1, 0x07, 0x04, 0x22, 0x44,
	0x21, 0xF3, 0xE0, 0x0F, 0x80, 0x00, 0x00, 0x79, 0xE1, 0xE3, 0xC7, 0x88,
	0x02, 0x44, 0x71, 0x22, 0x23, 0x88, 0x91, 0x1C, 0x40, 0x09, 0x03, 0xC1,
	0x86, 0x84, 0x36, 0x44, 0x09, 0xF0, 0xE1, 0x0E, 0x00, 0x00, 0xFD, 0x09,
	0xE1, 0x42, 0x1B, 0xC3, 0x18, 0x20, 0x01, 0x50, 0x80, 0x1F, 0xFF, 0x0C,
	0x30, 0x70, 0x40, 0x08, 0x01, 0x2A, 0xAA, 0x80, 0x04, 0x09, 0xC0, 0x05,
	0x00, 0xC0, 0x00, 0x00, 0x01, 0x47, 0x89, 0x92, 0x00, 0x20, 0x11, 0x50,
	0x81, 0x00, 0x00, 0x20, 0x44, 0x21, 0x02, 0x20, 0x88, 0x91, 0x10, 0x44,
	0x10, 0x00, 0x81, 0x00, 0x04, 0x00, 0x40, 0x89, 0x12, 0x24, 0x48, 0x10,
	0x22, 0x44, 0x21, 0x12, 0x44, 0x08, 0x91, 0x22, 0x40, 0x91, 0x22, 0x21,
	0x08, 0x8A, 0x36, 0x44, 0x21, 0x03, 0x00, 0x41, 0x80, 0x00, 0x00, 0x89,
	0x12, 0x04, 0x48, 0x08, 0x1E, 0x44, 0x20, 0x22, 0x41, 0x0A, 0x91, 0x22,
	0x78, 0x79, 0x00, 0x22, 0x49, 0x8A, 0x2A, 0x28, 0x78, 0x81, 0x81, 0x03,
	0x00, 0x2A, 0xFD, 0x09, 0xC0, 0x85, 0x15, 0xE7, 0xF0, 0x40, 0x02, 0x48,
	0x80, 0x0C, 0x7F, 0x1C, 0x38, 0xF9, 0xF8, 0x08, 0x00, 0x95, 0x55, 0x80,
	0x04, 0x0B, 0xE0, 0x1D, 0xCC, 0xC0, 0x00, 0x20, 0x03, 0xE1, 0x44, 0x15,
	0x00, 0x40, 0x08, 0xE0, 0x81, 0x00, 0x00, 0x10, 0x64, 0x20, 0x80, 0x27,
	0xC0, 0x91, 0x10, 0x44, 0x08, 0x40, 0x82, 0x0F, 0x82, 0x08, 0x58, 0xF9,
	0x12, 0x04, 0x48, 0x10, 0x26, 0x44, 0x20, 0x12, 0x84, 0x08, 0x93, 0x22,
	0x40, 0xA9, 0x40, 0x21, 0x08, 0x91, 0x2A, 0x28, 0x20, 0x83, 0x00, 0x81,
	0x91, 0x00, 0x00, 0x79, 0x12, 0x04, 0x4F, 0x88, 0x22, 0x44, 0x20, 0x23,
	0x81, 0x0A, 0x91, 0x22, 0x44, 0x89, 0x01, 0xC2, 0x08, 0x91, 0x2A, 0x10,
	0x88, 0x41, 0x81, 0x03, 0x00, 0x14, 0xFF, 0x05, 0x80, 0x88, 0x86, 0xE3,
	0x00, 0xFE, 0x00, 0x44, 0x90, 0x04, 0x7F, 0x3F, 0xFD, 0xFD, 0xF7, 0xF8,
	0x1F, 0xEA, 0xAA, 0x80, 0x04, 0x0F, 0xF0, 0x00, 0x0C, 0xC0, 0x00, 0x20,
	0x01, 0x43, 0x82, 0x08, 0x00, 0x40, 0x08, 0x43, 0xE0, 0x0F, 0x80, 0x08,
	0x54, 0x20, 0x60, 0xC4, 0x80, 0x9E, 0x08, 0x38, 0x78, 0x00, 0x04, 0x00,
	0x01, 0x08, 0x5C, 0x89, 0xE2, 0x04, 0x4F, 0x1E, 0x20, 0x7C, 0x20, 0x13,
	0x04, 0x0A, 0x95, 0x22, 0x78, 0x89, 0xE1, 0xC1, 0x08, 0x91, 0x2A, 0x10,
	0x20, 0x43, 0x01, 0x01, 0x8A, 0x00, 0x00, 0x09, 0x12, 0x04, 0x48, 0x9E,
	0x22, 0x44, 0x20, 0x22, 0x41, 0x0A, 0x91, 0x22, 0x44, 0x89, 0x82, 0x02,
	0x08, 0x91, 0x22, 0x28, 0x88, 0x23, 0x01, 0x01, 0x80, 0x2A, 0x6C, 0xD9,
	0x01, 0x40, 0x5F, 0x5F, 0xF0, 0x40, 0x00, 0x42, 0xA0, 0x00, 0x7F, 0x1C,
	0x38, 0x20, 0xE0, 0x08, 0x00, 0x95, 0x55, 0x83, 0xF8, 0x0B, 0xE0, 0x1D,
	0xC0, 0xC0, 0x00, 0x20, 0xA3, 0xE5, 0x01, 0x14, 0x08, 0x40, 0x08, 0xE0,
	0x80, 0x00, 0x00, 0x04, 0x4C, 0x20, 0x10, 0x42, 0x8F, 0x10, 0x04, 0x44,
	0x88, 0x40, 0x82, 0x0F, 0x82, 0x04, 0x54, 0x89, 0x12, 0x04, 0x48, 0x10,
	0x20, 0x44, 0x20, 0x12, 0x84, 0x0A, 0x99, 0x22, 0x44, 0x89, 0x12, 0x01,
	0x08, 0x91, 0x22, 0x28, 0x50, 0x23, 0x02, 0x01, 0x84, 0x00, 0x00, 0x71,
	0xE1, 0xE3, 0xC7, 0x08, 0x1C, 0x78, 0x60, 0x62, 0x21, 0x0D, 0x9E, 0x1C,
	0x78, 0x79, 0x71, 0xE7, 0x88, 0x91, 0x22, 0x44, 0x89, 0xF1, 0x81, 0x03,
	0x00, 0x14, 0x10, 0x20, 0x02, 0x20, 0x3F, 0x81, 0x8C, 0x20, 0x00, 0x41,
	0xC0, 0x00, 0x7F, 0x0C, 0x30, 0x24, 0x48, 0x08, 0x01, 0x2A, 0xAA, 0x7C,
	0x00, 0x09, 0xC0, 0x05, 0x00, 0xC0, 0x00, 0x20, 0xA1, 0x43, 0xCC, 0x94,
	0x08, 0x20, 0x11, 0x50, 0x80, 0x00, 0x00, 0x02, 0x44, 0x61, 0x10, 0x21,
	0x88, 0x08, 0x02, 0x44, 0x88, 0x00, 0x01, 0x00, 0x04, 0x22, 0x44, 0x51,
	0x12, 0x24, 0x48, 0x10, 0x20, 0x44, 0x20, 0x12, 0x44, 0x0D, 0x91, 0x22,
	0x44, 0x89, 0x12, 0x21, 0x08, 0x91, 0x22, 0x44, 0x88, 0x13, 0x04, 0x01,
	0x80, 0x00, 0x00, 0x01, 0x00, 0x00, 0x40, 0x09, 0x00, 0x40, 0x00, 0x02,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x81, 0x03, 0x16, 0x2A, 0x08, 0x10, 0x07, 0xF0, 0x1F, 0xC3,
	0x80, 0x10, 0x00, 0x40, 0x8F, 0xE0, 0x7F, 0x64, 0x26, 0x04, 0x08, 0x08,
	0x02, 0x15, 0x55, 0x00, 0x00, 0x08, 0x8F, 0xE5, 0x3F, 0xC0, 0x00, 0x20,
	0xA1, 0x41, 0x0C, 0x08, 0x08, 0x10, 0x20, 0x40, 0x00, 0x00, 0x00, 0x00,
	0x38, 0x20, 0xE3, 0xE0, 0x8F, 0x87, 0x3E, 0x38, 0x70, 0x00, 0x00, 0x80,
	0x08, 0x1C, 0x38, 0x21, 0xE1, 0xC7, 0x8F, 0x9F, 0x1E, 0x44, 0x70, 0x12,
	0x24, 0x08, 0x91, 0x1C, 0x78, 0x71, 0xE1, 0xC7, 0xC8, 0x91, 0x22, 0x44,
	0x89, 0xF3, 0xE0, 0x0F, 0x80, 0x00, 0x00, 0x01, 0x00, 0x00, 0x40, 0x06,
	0x00, 0x40, 0x20, 0x22, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE1, 0x0E, 0x0D, 0x00
};


const unsigned char inversefont[958] = {
	0x42, 0x4D, 0xBE, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00,
	0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0x08, 0x00,
	0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03,
	0x00, 0x00, 0x13, 0x0B, 0x00, 0x00, 0x13, 0x0B, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
	0xFF, 0x00, 0x93, 0x26, 0xF0, 0x0F, 0xE0, 0x2F, 0xF8, 0xFF, 0x57, 0xBF,
	0x7F, 0xFB, 0xC0, 0x84, 0x21, 0xFB, 0xF7, 0xF0, 0x1F, 0xD5, 0x55, 0xFF,
	0xFF, 0xF7, 0xF0, 0x1F, 0xC0, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE3, 0xFF, 0xFF, 0x3F,
	0xFF, 0xFF, 0xFF, 0xFF, 0xBF, 0xF7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x8F, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0x81, 0x5A, 0x4D, 0x5D, 0xE4, 0x00,
	0xF7, 0xEF, 0xFF, 0x1F, 0x7F, 0xF3, 0xC0, 0xFB, 0xDF, 0xDB, 0xB7, 0xF7,
	0xFD, 0xEA, 0xAA, 0x00, 0x03, 0xF7, 0x7F, 0xFA, 0xFF, 0x3F, 0xFF, 0xDF,
	0xFE, 0xBE, 0xFE, 0x72, 0xFF, 0xEF, 0xDF, 0xBF, 0xFD, 0xFF, 0xFB, 0xFF,
	0xC7, 0x8E, 0x0E, 0x3F, 0x78, 0xF1, 0xEF, 0xC7, 0x1F, 0xFE, 0xFF, 0x7F,
	0xF7, 0xF7, 0xC3, 0x76, 0x1E, 0x38, 0x70, 0x6F, 0xE1, 0xBB, 0x8F, 0x1D,
	0xD8, 0x37, 0x6E, 0xE3, 0xBF, 0x96, 0xEE, 0x3E, 0xF8, 0xFB, 0xDD, 0xBB,
	0xDE, 0x0C, 0x1F, 0xF0, 0x7F, 0xFF, 0xFF, 0x86, 0x1E, 0x1C, 0x38, 0x77,
	0xFD, 0xBB, 0x8E, 0xDD, 0xDC, 0x77, 0x6E, 0xE3, 0xBF, 0xF6, 0xFC, 0x3E,
	0x79, 0x7B, 0xC9, 0xBB, 0xF6, 0x0F, 0x1E, 0xF1, 0xFF, 0xFF, 0x02, 0xF6,
	0x1E, 0xBD, 0xE4, 0x3C, 0xE7, 0xDF, 0xFE, 0xAF, 0x7F, 0xE0, 0x00, 0xF3,
	0xCF, 0x8F, 0xBF, 0xF7, 0xFE, 0xD5, 0x55, 0x7F, 0xFB, 0xF6, 0x3F, 0xFA,
	0xFF, 0x3F, 0xFF, 0xFF, 0xFE, 0xB8, 0x76, 0x6D, 0xFF, 0xDF, 0xEE, 0xAF,
	0x7E, 0xFF, 0xFF, 0xDF, 0xBB, 0xDE, 0xFD, 0xDF, 0x77, 0x6E, 0xEF, 0xBB,
	0xEF, 0xFF, 0x7E, 0xFF, 0xFB, 0xFF, 0xBF, 0x76, 0xED, 0xDB, 0xB7, 0xEF,
	0xDD, 0xBB, 0xDE, 0xED, 0xBB, 0xF7, 0x6E, 0xDD, 0xBF, 0x6E, 0xDD, 0xDE,
	0xF7, 0x75, 0xC9, 0xBB, 0xDE, 0xFC, 0xFF, 0xBE, 0x7F, 0xFF, 0xFF, 0x76,
	0xED, 0xFB, 0xB7, 0xF7, 0xE1, 0xBB, 0xDF, 0xDD, 0xBE, 0xF5, 0x6E, 0xDD,
	0x87, 0x86, 0xFF, 0xDD, 0xB6, 0x75, 0xD5, 0xD7, 0x87, 0x7E, 0x7E, 0xFC,
	0xFF, 0xD5, 0x02, 0xF6, 0x3F, 0x7A, 0xEA, 0x18, 0x0F, 0xBF, 0xFD, 0xB7,
	0x7F, 0xF3, 0x80, 0xE3, 0xC7, 0x06, 0x07, 0xF7, 0xFF, 0x6A, 0xAA, 0x7F,
	0xFB, 0xF4, 0x1F, 0xE2, 0x33, 0x3F, 0xFF, 0xDF, 0xFC, 0x1E, 0xBB, 0xEA,
	0xFF, 0xBF, 0xF7, 0x1F, 0x7E, 0xFF, 0xFF, 0xEF, 0x9B, 0xDF, 0x7F, 0xD8,
	0x3F, 0x6E, 0xEF, 0xBB, 0xF7, 0xBF, 0x7D, 0xF0, 0x7D, 0xF7, 0xA7, 0x06,
	0xED, 0xFB, 0xB7, 0xEF, 0xD9, 0xBB, 0xDF, 0xED, 0x7B, 0xF7, 0x6C, 0xDD,
	0xBF, 0x56, 0xBF, 0xDE, 0xF7, 0x6E, 0xD5, 0xD7, 0xDF, 0x7C, 0xFF, 0x7E,
	0x6E, 0xFF, 0xFF, 0x86, 0xED, 0xFB, 0xB0, 0x77, 0xDD, 0xBB, 0xDF, 0xDC,
	0x7E, 0xF5, 0x6E, 0xDD, 0xBB, 0x76, 0xFE, 0x3D, 0xF7, 0x6E, 0xD5, 0xEF,
	0x77, 0xBE, 0x7E, 0xFC, 0xFF, 0xEB, 0x00, 0xFA, 0x7F, 0x77, 0x79, 0x1C,
	0xFF, 0x01, 0xFF, 0xBB, 0x6F, 0xFB, 0x80, 0xC0, 0x02, 0x02, 0x08, 0x07,
	0xE0, 0x15, 0x55, 0x7F, 0xFB, 0xF0, 0x0F, 0xFF, 0xF3, 0x3F, 0xFF, 0xDF,
	0xFE, 0xBC, 0x7D, 0xF7, 0xFF, 0xBF, 0xF7, 0xBC, 0x1F, 0xF0, 0x7F, 0xF7,
	0xAB, 0xDF, 0x9F, 0x3B, 0x7F, 0x61, 0xF7, 0xC7, 0x87, 0xFF, 0xFB, 0xFF,
	0xFE, 0xF7, 0xA3, 0x76, 0x1D, 0xFB, 0xB0, 0xE1, 0xDF, 0x83, 0xDF, 0xEC,
	0xFB, 0xF5, 0x6A, 0xDD, 0x87, 0x76, 0x1E, 0x3E, 0xF7, 0x6E, 0xD5, 0xEF,
	0xDF, 0xBC, 0xFE, 0xFE, 0x75, 0xFF, 0xFF, 0xF6, 0xED, 0xFB, 0xB7, 0x61,
	0xDD, 0xBB, 0xDF, 0xDD, 0xBE, 0xF5, 0x6E, 0xDD, 0xBB, 0x76, 0x7D, 0xFD,
	0xF7, 0x6E, 0xDD, 0xD7, 0x77, 0xDC, 0xFE, 0xFE, 0x7F, 0xD5, 0x93, 0x26,
	0xFE, 0xBF, 0xA0, 0xA0, 0x0F, 0xBF, 0xFF, 0xBD, 0x5F, 0xFF, 0x80, 0xE3,
	0xC7, 0xDF, 0x1F, 0xF7, 0xFF, 0x6A, 0xAA, 0x7C, 0x07, 0xF4, 0x1F, 0xE2,
	0x3F, 0x3F, 0xFF, 0xDF, 0x5C, 0x1A, 0xFE, 0xEB, 0xF7, 0xBF, 0xF7, 0x1F,
	0x7F, 0xFF, 0xFF, 0xFB, 0xB3, 0xDF, 0xEF, 0xBD, 0x70, 0xEF, 0xFB, 0xBB,
	0x77, 0xBF, 0x7D, 0xF0, 0x7D, 0xFB, 0xAB, 0x76, 0xED, 0xFB, 0xB7, 0xEF,
	0xDF, 0xBB, 0xDF, 0xED, 0x7B, 0xF5, 0x66, 0xDD, 0xBB, 0x76, 0xED, 0xFE,
	0xF7, 0x6E, 0xDD, 0xD7, 0xAF, 0xDC, 0xFD, 0xFE, 0x7B, 0xFF, 0xFF, 0x8E,
	0x1E, 0x1C, 0x38, 0xF7, 0xE3, 0x87, 0x9F, 0x9D, 0xDE, 0xF2, 0x61, 0xE3,
	0x87, 0x86, 0x8E, 0x18, 0x77, 0x6E, 0xDD, 0xBB, 0x76, 0x0E, 0x7E, 0xFC,
	0xFF, 0xEB, 0xEF, 0xDF, 0xFD, 0xDF, 0xC0, 0x7E, 0x73, 0xDF, 0xFF, 0xBE,
	0x3F, 0xFF, 0x80, 0xF3, 0xCF, 0xDB, 0xB7, 0xF7, 0xFE, 0xD5, 0x55, 0x83,
	0xFF, 0xF6, 0x3F, 0xFA, 0xFF, 0x3F, 0xFF, 0xDF, 0x5E, 0xBC, 0x33, 0x6B,
	0xF7, 0xDF, 0xEE, 0xAF, 0x7F, 0xFF, 0xFF, 0xFD, 0xBB, 0x9E, 0xEF, 0xDE,
	0x77, 0xF7, 0xFD, 0xBB, 0x77, 0xFF, 0xFE, 0xFF, 0xFB, 0xDD, 0xBB, 0xAE,
	0xED, 0xDB, 0xB7, 0xEF, 0xDF, 0xBB, 0xDF, 0xED, 0xBB, 0xF2, 0x6E, 0xDD,
	0xBB, 0x76, 0xED, 0xDE, 0xF7, 0x6E, 0xDD, 0xBB, 0x77, 0xEC, 0xFB, 0xFE,
	0x7F, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xBF, 0xF6, 0xFF, 0xBF, 0xFF, 0xFD,
	0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFE, 0x7E, 0xFC, 0xE9, 0xD5, 0xF7, 0xEF, 0xF8, 0x0F, 0xE0, 0x3C,
	0x7F, 0xEF, 0xFF, 0xBF, 0x70, 0x1F, 0x80, 0x9B, 0xD9, 0xFB, 0xF7, 0xF7,
	0xFD, 0xEA, 0xAA, 0xFF, 0xFF, 0xF7, 0x70, 0x1A, 0xC0, 0x3F, 0xFF, 0xDF,
	0x5E, 0xBE, 0xF3, 0xF7, 0xF7, 0xEF, 0xDF, 0xBF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xC7, 0xDF, 0x1C, 0x1F, 0x70, 0x78, 0xC1, 0xC7, 0x8F, 0xFF, 0xFF, 0x7F,
	0xF7, 0xE3, 0xC7, 0xDE, 0x1E, 0x38, 0x70, 0x60, 0xE1, 0xBB, 0x8F, 0xED,
	0xDB, 0xF7, 0x6E, 0xE3, 0x87, 0x8E, 0x1E, 0x38, 0x37, 0x6E, 0xDD, 0xBB,
	0x76, 0x0C, 0x1F, 0xF0, 0x7F, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xBF, 0xF9,
	0xFF, 0xBF, 0xDF, 0xDD, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1E, 0xF1, 0xF2, 0xFF
};

void af_Create()
{
	int w, h;
	//unsigned char* img = read_bmp("font-normal.bmp", &w, &h);
	unsigned char* img = af_read_bmp_memory((char*)normalfont, &w, &h);

	for (int n = 0; n < FONT_NUM; n++)
	{
		int pos = 0;
		for (int y = 0; y < FONT_Y; y++)
			for (int x = 0; x < FONT_X; x++)
				af_font[n][pos++] = img[y * w + (x + n * FONT_X)];
 	}
	free(img);

	//img = read_bmp("font-reverse.bmp", &w, &h);
	img = af_read_bmp_memory((char*)inversefont, &w, &h);
	for (int n = 0; n < FONT_NUM; n++)
	{
		int pos = 0;
		for (int y = 0; y < FONT_Y; y++)
			for (int x = 0; x < FONT_X; x++)
				af_invfont[n][pos++] = img[y * w + (x + n * FONT_X)];
	}

	free(img);

}

void af_RenderFont(unsigned char* backbuffer, int fontnum, int posx, int posy, bool inv)
{
	int pos = 0;
	for(int y=0; y<FONT_Y; y++)
		for (int x = 0; x < FONT_X; x++)
		{
			unsigned char c = inv ? af_invfont[fontnum][pos++] : af_font[fontnum][pos++];
			if (c == 1)
			{
				// if(backbuffer == NULL)
				// 	DrawPixel(posx + x, posy + y, GREEN);
				// else
				// 	backbuffer[((posy+y)*SCREENSIZE_X) + (posx + x)] = GREEN;
				backbuffer[((posy+y)*SCREENSIZE_X) + (posx + x)] = 15;	// white
			}
			else
			{
				// if (backbuffer == NULL)
				// 	DrawPixel(posx + x, posy + y, BLACK);
				// else
					backbuffer[((posy + y) * SCREENSIZE_X) + (posx + x)] = 0;	// black
			}
		}
}


// unsigned char* af_read_bmp(const char* fname, int* _w, int* _h)
// {
// 	unsigned char head[54];
// 	FILE* f = NULL;
// 	fopen_s(&f, fname, "rb");

// 	// BMP header
// 	fread(head, 1, 54, f);

// 	int w = head[18] + (((int)head[19]) << 8) + (((int)head[20]) << 16) + (((int)head[21]) << 24);
// 	int h = head[22] + (((int)head[23]) << 8) + (((int)head[24]) << 16) + (((int)head[25]) << 24);

// 	// lines are aligned on 4-byte boundary
// 	int lineSize = (w / 8 + (w / 8) % 4);
// 	int fileSize = lineSize * h;

// 	unsigned char* img = (unsigned char*)malloc(w * h);
// 	unsigned char* data = (unsigned char*)malloc(fileSize);

// 	// skip
// 	fseek(f, 54, SEEK_SET);
// 	// skip palette
// 	fseek(f, 8, SEEK_CUR);

// 	fread(data, 1, fileSize, f);

// 	// decode bits
// 	int i, j, k, rev_j;
// 	for (j = 0, rev_j = h - 1; j < h; j++, rev_j--)
// 	{
// 		for (i = 0; i < w / 8; i++) {
// 			int fpos = j * lineSize + i, pos = rev_j * w + i * 8;
// 			for (k = 0; k < 8; k++)
// 				img[pos + (7 - k)] = (data[fpos] >> k) & 1;
// 		}
// 	}

// 	free(data);
// 	*_w = w; *_h = h;
// 	return img;
// }


unsigned char* af_read_bmp_memory(char* buffer, int* _w, int* _h)
{
	unsigned char head[54];
	int pos = 0;

	memcpy(head, buffer, 54);

	int w = head[18] + (((int)head[19]) << 8) + (((int)head[20]) << 16) + (((int)head[21]) << 24);
	int h = head[22] + (((int)head[23]) << 8) + (((int)head[24]) << 16) + (((int)head[25]) << 24);

	// lines are aligned on 4-byte boundary
	int lineSize = (w / 8 + (w / 8) % 4);
	int fileSize = lineSize * h;

	unsigned char* img = (unsigned char*)malloc(w * h);
	unsigned char* data = (unsigned char*)malloc(fileSize);

	// skip
	pos = 54;
	pos += 8;

	memcpy(data, buffer + pos, fileSize);

	// decode bits
	int i, j, k, rev_j;
	for (j = 0, rev_j = h - 1; j < h; j++, rev_j--)
	{
		for (i = 0; i < w / 8; i++) {
			int fpos = j * lineSize + i, pos = rev_j * w + i * 8;
			for (k = 0; k < 8; k++)
				img[pos + (7 - k)] = (data[fpos] >> k) & 1;
		}
	}

	free(data);
	*_w = w; *_h = h;
	return img;
}

