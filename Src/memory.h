#ifndef MEMORY_H
#define MEMORY_H

#include "kerneldef.h"
#include <stddef.h>

struct m_header
{
    struct m_header* prev;
    struct m_header* next;
    size_t length;
    bool used;
};
void k_init_mem();

typedef struct m_header m_header_t;

void k_init_mem();
void* malloc(size_t size);
void free(void* ptr);
void* calloc(size_t num, size_t size);
void* realloc(void* ptr, size_t size);
void* memset(void* ptr, int value, size_t size);
void* memcpy(void* restrict dst, const void* src, size_t size); 

void dump_heap(void);

void displayMemoryInfo();

#endif
