
#include "console.h"
#include "vga.h"
#include "timer.h"
#include "isr.h"
#include "util.h"
#include "memory.h"

#include "../Apple2/AppleMachine.h"

//#define TARGET_FRAME 30
#define WINDOW_SIZE_X 320
#define WINDOW_SIZE_Y 200


static unsigned char g_memory[MAX_MEM_SIZE];
static int frame = 0;

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
	k_sleep(1500*20);
	//malloctest();
}

void init_A2()
{
	k_setVgaMode(WINDOW_SIZE_X, WINDOW_SIZE_Y, 256);
	k_clearVga(0);
	machine_InitMachine();
}

void kernel_loop()
{
	unsigned long p = 17050;
	//unsigned long p = 1000;
	machine_Run(p);

	// Render
	machine_Render(frame);
	if (frame++ > TARGET_FRAME) 
	 	frame = 0;
}

#if 0
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
#endif

void kmain(void)
{
    init_kernel();
	init_A2();
	//runDemo();
	while(1)
	{
		kernel_loop();
	};
}