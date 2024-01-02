
#include "console.h"
#include "vga.h"
#include "timer.h"
#include "isr.h"

void print_logo() 
{
	kprint("########   #######           #######   ######\n",0,1);
	kprint("##     ## ##     ##         ##     ## ##    ##\n",0,2);
	kprint("##     ##        ##         ##     ## ##\n",0,3);
	kprint("########   #######  ####### ##     ##  ######\n",0,4);
	kprint("##     ## ##                ##     ##       ##\n",0,5);
	kprint("##     ## ##                ##     ## ##    ##\n", 0, 6);
	kprint("########  #########          #######   ######\n", 0, 7);
}

void init()
{
	kisr_install();
	kinitTimer(1193);
    kinit_console();
	kclear_console();
    print_logo();
}

void kmain(void)
{
    init();

	ksleep(1000);
    setVgaMode(320, 200, 256);

	unsigned char color = 0;
	while(1) 
	{
		clearVga(color++);
		ksleep(100);
		color = color >= 64 ? 0 : color;
	};
}