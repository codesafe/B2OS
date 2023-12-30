#include "low_level.h"

unsigned char port_8_in(unsigned short port)
{
    unsigned char ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

void port_8_out(unsigned short port, unsigned char data)
{
     asm volatile("outb %0, %1" : : "a"(data), "Nd"(port) );
}

///////////////////////////////////////

unsigned char port_byte_in(unsigned short port)
{
  // C wrapper function that reads a byte from the specified port
  // "=a" (result) means: Put AL register in variable result when finished
  // "d" (port) means: Load EDX with port variable
  unsigned char result;
  __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
  return result;
}

void port_byte_out(unsigned short port, unsigned char data)
{
  // "a" (data) means: Load EAX with data
  // "d" (port) means: Load EDX with port
  // Notice the semicolons mean output and input
  __asm__("out %%al, %%dx" : : "a" (data), "d" (port));
}

unsigned short port_word_in(unsigned short port)
{
  unsigned short result;
  __asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));
  return result;
}

void port_word_out(unsigned short port, unsigned char data)
{
  __asm__("out %%ax, %%dx" : : "a" (data), "d" (port));
}
