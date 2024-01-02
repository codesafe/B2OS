#ifndef PREDEF_H
#define PREDEF_H

#include <stdint.h>

typedef char bool;
#define true 1
#define false 0

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

/* Struct which aggregates many registers */
typedef struct 
{
   unsigned int ds; /* Data segment selector */
   unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax; /* Pushed by pusha. */
   unsigned int int_no, err_code; /* Interrupt number and error code (if applicable) */
   unsigned int eip, cs, eflags, useresp, ss; /* Pushed by the processor automatically */
} registers_t;


#define low_16(address) (uint16_t)((address) & 0xFFFF)
#define high_16(address) (uint16_t)(((address) >> 16) & 0xFFFF)


#endif