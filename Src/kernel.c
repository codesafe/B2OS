
#include "console.h"
#include "vga.h"
#include "timer.h"
#include "isr.h"
#include "util.h"
#include "memory.h"

#include "../Apple2/AppleMachine.h"

//#define TARGET_FRAME 30
#define WINDOW_SIZE_X 640
#define WINDOW_SIZE_Y 480

//static unsigned char g_memory[MAX_MEM_SIZE];
static int frame = 0;

void print_logo() 
{
	printf("########   #######           #######   ######\n");
	printf("##     ## ##     ##         ##     ## ##    ##\n");
	printf("##     ##        ##         ##     ## ##\n");
	printf("########   #######  ####### ##     ##  ######\n");
	printf("##     ## ##                ##     ##       ##\n");
	printf("##     ## ##                ##     ## ##    ##\n");
	printf("########  #########          #######   ######\n");
	printf(" \n");
}

void malloctest()
{
	for(int i=0; i<100; i++)
	{
		int size = k_rand() % 1000;
		void *ptr = malloc(size);
		printf("MALLOC\n");
		free(ptr);
		k_sleep(100);
		printf("FREE\n");
		k_sleep(100);
	}
}

void init_kernel()
{
	console_init(COLOR_WHITE, COLOR_BLACK);
	console_clear(COLOR_WHITE, COLOR_BLACK);
    //print_logo();
	k_init_isr();
	k_init_timer(500);
	//k_sleep(100);
	k_init_mem();
	//displayMemoryInfo();
	k_srand(global_tick);
	k_sleep(500);
	//malloctest();
}

void init_A2()
{
	//k_setVgaMode(WINDOW_SIZE_X, WINDOW_SIZE_Y, 24);
	//k_clearVga(0xFF00FF00);
	//k_swapBuffer();
	machine_InitMachine();
}

void kernel_loop()
{
	//unsigned long p = 17050;
	unsigned long p = 3050;	
	machine_Run(p);
	printf("Update Loop %d\n", frame);

	// Render
	machine_Render(frame);
	if (frame++ > TARGET_FRAME) 
	 	frame = 0;

	printf("Render Loop %d\n", frame);		
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

		// unsigned char color = k_rand() % 100;
		// int x = k_rand() % WINDOW_SIZE_X;
		// int y = k_rand() % WINDOW_SIZE_Y;
		// k_drawPixel(x, y, color);


	};
}