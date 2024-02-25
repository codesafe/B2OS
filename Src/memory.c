
//#include "io.h"
//#include "string.h"
#include "kerneldef.h"
#include "memory.h"
#include "console.h"

//static __attribute__((aligned(4096))) uint32_t pdt[1024];
static const size_t header_len = sizeof(m_header_t);
static m_header_t* head = NULL;
//static semaphore_t mem_sema;

// current limitation: can only add one contiguous block of memory to the heap
void k_init_mem(void* ptr, size_t size) 
{
    if (head || size < header_len) return;
    //sema_init(&mem_sema, 1);

    head = (m_header_t*) ptr;
    head->prev = head->next = NULL;
    head->used = false;
    head->length = size - header_len;

    printf("[INIT MEMORY]\n");
}

// lame first-fit allocator
void* malloc(size_t size) 
{
    if (!size) return NULL;
    // round size up to 4 byte alignment
    if (size & 0b11) size = (size & ~0b11) + 4;
    //sema_down(&mem_sema);

    m_header_t* ptr = head;
    unsigned char* result = NULL;
    while (ptr) 
    {
        if (!ptr->used && ptr->length >= size) 
        {
            if (size + header_len <= ptr->length) 
            {
                m_header_t* split = (m_header_t*) ((unsigned char*) ptr + header_len + size);
                split->prev = ptr;
                split->next = ptr->next;
                ptr->next = split;
                split->used = false;
                split->length = ptr->length - size - header_len;
                ptr->length = size;
            }
            ptr->used = true;
            result = (unsigned char*) ptr + header_len;
            break;
        } 
        else 
        ptr = ptr->next;
    }
    //sema_up(&mem_sema);
    return result;
}

void free(void* ptr) 
{
    if (!ptr) return;
    //sema_down(&mem_sema);
    m_header_t* entry = (m_header_t*) ((unsigned char*) ptr - header_len);
    entry->used = false;
    // merge adjacent blocks
    // assumes that two adjacent blocks are right after each other in memory
    if (entry->next && !entry->next->used) 
    {
        entry->length += header_len + entry->next->length;
        entry->next = entry->next->next;
        if (entry->next) 
        {
            entry->next->prev = entry;
        }
    }
    if (entry->prev && !entry->prev->used) 
    {
        entry->prev->length += header_len + entry->length;
        entry->prev->next = entry->next;
        if (entry->next) 
        {
            entry->next->prev = entry->prev;
        }
    }
    //sema_up(&mem_sema);
}

void* calloc(size_t num, size_t size) 
{
    void* buf = malloc(num * size);
    if (buf) memset(buf, 0, num * size);
    return buf;
}

void* realloc(void* ptr, size_t size) 
{
    if (!ptr) return malloc(size);
    void* buf = malloc(size);
    if (!buf) return NULL;
    m_header_t* entry = (m_header_t*) ((unsigned char*) ptr - header_len);
    size_t orig_size = entry->length;
    size_t min_size = size > orig_size ? orig_size : size;
    memcpy(buf, ptr, min_size);
    free(ptr);
    return buf;
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

void dump_heap(void) 
{
    /*
    malloc_header_t* ptr = head;
    printf("*** heap dump: ***\n");
    if (!ptr) {
        printf("no entries\n");
    } else {
        malloc_header_t* last = NULL;
        while (ptr) {
            printf("entry: 0x%x, length: 0x%x%s\n", (uint32_t) ptr, ptr->length, ptr->used ? " (*)" : "");
            if (ptr->prev != last) printf("invalid back ptr to 0x%x\n", (uint32_t) ptr->prev);
            last = ptr;
            ptr = ptr->next;
        }
    }
    */
}

/*
void test_enable_paging(void) 
{
    memset(pdt, 0, sizeof(pdt));
    pdt[0] = 0x83;
    asm volatile(
        "mov %0, %%cr3\n\t"
        "mov %%cr4, %%eax\n\t"
        "or $0x00000010, %%eax\n\t"
        "mov %%eax, %%cr4\n\t"
        "mov %%cr0, %%eax\n\t"
        "or $0x80000000, %%eax\n\t"
        "mov %%eax, %%cr0"
        :
        : "r"(pdt)
        : "eax"
    );
}
*/

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
