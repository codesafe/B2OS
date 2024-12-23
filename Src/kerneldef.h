#ifndef KERNELDEF_H
#define KERNELDEF_H

//#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef signed char sint8;
typedef signed short sint16;
typedef signed int sint32;
typedef uint8 byte;
typedef uint16 word;
typedef uint32 dword;


//typedef char bool;
//#define true 1
//#define false 0

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

typedef struct 
{
   unsigned int ds; /* Data segment selector */
   unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax; /* Pushed by pusha. */
   unsigned int int_no, err_code; /* Interrupt number and error code (if applicable) */
   unsigned int eip, cs, eflags, useresp, ss; /* Pushed by the processor automatically */
} registers_t;


#define low_16(address) (unsigned short)((address) & 0xFFFF)
#define high_16(address) (unsigned short)(((address) >> 16) & 0xFFFF)

#define MAX_MEM_SIZE    1024*1024*10 // ( 10MB )

#define panic_line(...) panic(__func__, __LINE__, __VA_ARGS__)


#endif