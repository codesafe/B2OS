#ifndef MEMORY_H
#define MEMORY_H

#include "kerneldef.h"
#include <stddef.h>

void k_init_mem();

void* malloc(size_t size);
void free(void* ptr);
void* memset(void* ptr, int value, size_t size);
void* memcpy(void* restrict dst, const void* src, size_t size); 

void displayMemoryInfo();

#endif