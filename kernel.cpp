#include "vga.h"

extern "C" void main()
{
    VGA vga;

    vga.terminal_initialize();
    vga.print_welcome_msg();    

    *(char*)0xb8000 = 'Q';

	while(1) 
    {
		#ifdef GRAPHICS_MODE
            desktop.draw(&render);
			// Display rendered frame
			render.display(&vga);
        #endif
	}

    //return;
}