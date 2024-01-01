//#include "../Driver/vga.h"
//#include "screen.h"
#include "console.h"
//#include "vga.h"

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
    kinit_console();
    print_logo();
}

void main(void)
{
    init();

    //setMode(320, 200, 256);

	while(1) {};

    //return;
}