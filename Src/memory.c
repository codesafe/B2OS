
#include "kerneldef.h"
#include "memory.h"
#include "console.h"
#include "memory/heap.h"
#include "memory/page.h"

void k_init_mem() 
{
    printf("[INIT MEMORY]\n");

    paging_init();
    heap_init();
}

void* malloc(size_t size) 
{
    return heap_alloc(size, 8);
}

void free(void* ptr) 
{
    heap_free(ptr);
}


void* memset(void* ptr, int value, size_t size) 
{
    unsigned char* buf = (unsigned char*) ptr;
    for (size_t i = 0; i < size; i++) 
    {
        buf[i] = (unsigned char) value;
    }
    return ptr;
}

void* memcpy(void* restrict dst, const void* src, size_t size) 
{
    unsigned char* restrict _dst = (unsigned char* restrict) dst;
    const unsigned char* _src = (const unsigned char*) src;
    for (size_t i = 0; i < size; i++) 
    {
        _dst[i] = _src[i];
    }
    return dst;
}


////////////////////////////////////////////////////////////////////////

typedef struct 
{
    unsigned short lower_size;
    unsigned short upper_size;
} mem_t;

mem_t memget()
{                     
    // memory.asm -> memory_detection 
    mem_t* mem = (mem_t*)0x5000;
    return *mem;
}

mem_t upper_memget ()
{
    // memory.asm -> upper_mem_map
    mem_t* mem = (mem_t*)0x5100;
    return *mem;
}


// int getUsedDynamicMem()
// {
// //     kprint(toString(getFreeMem() - 0x10000, 10));
//     //kprintInt(getFreeMem() - 0x10000);
//     int ret = getFreeMem() - 0x10000;
//     return ret;
// }

void displayMemoryInfo()
{
    char tmp[64];
    //k_print("meminfo:\n");
    //kprint(" e820h:\n");
    mem_t mem = memget();
    printf("Lower memory: \0");

    //int l = k_num2ascii(mem.lower_size, tmp);
    printf(tmp);
    //kprintInt (mem.lower_size);
    printf(" KB\n\0");

    printf("  Upper memory: ");
    //kprintInt (mem.upper_size);
    //k_num2ascii(mem.upper_size, tmp);
    printf(tmp);
    printf(" KB\n\0");
    

    //kprint("e801h:\n");
    mem = upper_memget();
    printf("Extended 1: \0");
    //kprintInt(mem.lower_size);
    //k_num2ascii(mem.lower_size, tmp);
    printf(tmp);
    printf(" KB\n\0");

    printf("Extended 2: \0");
    //kprintInt (mem.upper_size * 64);
    //k_num2ascii(mem.upper_size * 64, tmp);
    printf(tmp);    
    printf(" KB\n\0");
    
    //kprint("\n\nExtended 1 = Number of contiguous KB between 1 and 16 MB,");
    //kprint("\n             maximum 0x3C00 = 15 MB");
    //kprint("\n\nExtended 2 = Number of contiguous KB between 16 MB and 4 GB.");
}

unsigned short get_CMOS_memory_size()
{
    unsigned short total;
    unsigned char lowmem, highmem;
 
    // outb(0x70, 0x30);
    // lowmem = inb(0x71);
    // outb(0x70, 0x31);
    // highmem = inb(0x71);
 
    total = lowmem | highmem << 8;
    return total;
}


int getCMOSMem()
{
    unsigned char mem_size = get_CMOS_memory_size();
//     kprint(toString(mem_size, 10)); //mem.upper_size contains the size in 64K blocks
    //kprintInt(mem_size);
    //kprint(" KB\n");
    return mem_size;
}
