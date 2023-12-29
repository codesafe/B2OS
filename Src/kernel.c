
#include "../Driver/vga.h"

void init()
{

}

void main()
{
    init();

    *(char*)0xb8000 = 'Q';
    *(char*)0xb8002 = 'A';
    return;
}