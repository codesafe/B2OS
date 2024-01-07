
#include "console.h"
#include "vga.h"
#include "timer.h"
#include "isr.h"
#include "util.h"
#include "memory.h"

static unsigned char g_memory[MAX_MEM_SIZE];

void print_logo() 
{
	k_print("########   #######           #######   ######\n");
	k_print("##     ## ##     ##         ##     ## ##    ##\n");
	k_print("##     ##        ##         ##     ## ##\n");
	k_print("########   #######  ####### ##     ##  ######\n");
	k_print("##     ## ##                ##     ##       ##\n");
	k_print("##     ## ##                ##     ## ##    ##\n");
	k_print("########  #########          #######   ######\n");
	k_print(" \n");
}

void malloctest()
{
	for(int i=0; i<100; i++)
	{
		int size = k_rand() % 1000;
		void *ptr = malloc(size);
		k_print("MALLOC\n");
		free(ptr);
		k_sleep(100);
		k_print("FREE\n");
		k_sleep(100);
	}
}

void init_kernel()
{
	k_init_console();
	k_clear_console();
    print_logo();
	k_init_isr();
	k_init_timer(1000);
	//k_sleep(100);
	k_init_mem(g_memory, MAX_MEM_SIZE);
	//displayMemoryInfo();
	k_srand(global_tick);
	k_sleep(1500);
	//malloctest();
}

int graphic_width = 320;
int graphic_height = 200;

void runDemo()
{
    k_setVgaMode(graphic_width, graphic_height, 256);
	k_clearVga(0);

	while(1) 
	{
		unsigned char color = k_rand() % 100;
		int x = k_rand() % graphic_width;
		int y = k_rand() % graphic_height;
		k_drawPixel(x, y, color);
		//k_sleep(10);
	};
}

void kmain(void)
{
    init_kernel();
	//runDemo();
	while(1){};
}