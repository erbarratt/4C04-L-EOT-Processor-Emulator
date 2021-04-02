/**
*
* 4c04 L EOT Processor Emulator
*
* The 4c04 is a fictional virtual CPU, complete with it's own assembly language.
*
* It's a true 8-bit (byte) processor; it can only address memory locations 0-255 (256 bytes),
* and store 8 bits on one of four addressable registers.
*
* The memory is laid out as follows:
*
* 	- 0x00 - 0x6F is 112 bytes of none reserved RAM
* 	- 0x70 - 0x7F is 16 bytes of reserved Stack memory
* 	- 0x80 - 0xFF is 128 bytes of reserved ROM for program logic and data
*
* This application mounts the 4c04 program through the use of an included assembler. Put your assembly in a file called "prog.txt" in
* the same directory as the executable, and that will be assembled into the ROM area of memory.
*
* Details for the Assembly syntax can be found in draw.c
*
* To compile, you must link the X11 libraries
* On Linux:
* 		sudo apt-get install -y libx11-dev
*
* The emulator requires an X11 enabled desktop environment.
* On Linux:
* 		sudo apt install x11-apps
* On Windows:
* 		Install XMing
* 		Install Putty
* 		Enable and Install Ubuntu for WSL (Windows Subsystem for Linux)
* 		Install X11 on Ubuntu
* 		Configure Putty to forward X11
* 		Launch via Putty from Ubuntu (./4c04)
*
*/

#include <unistd.h> //usleep
#include "4c04.h"

int main(void){

	console_print("4c04 L EOT CPU Emulator\nSystem Booting...\n", 0);

	system_restart();
	
	program_load();
	
	console_print("Program Loaded...\n", 0);
	
	code_disassemble();
	
	console_print("Opening Window...\n", 0);
	
	Display *display;
	Window window;
	XEvent evnt;
	int screen;
	XFontStruct* font;
	GC gc;
	
	//open a connection to the x server that controls a display
	//NULL uses value of the DISPLAY env var
		display = XOpenDisplay(NULL);
		
	//if not set, then probably on only terminal.
		if (display == NULL) {
			console_print("Cannot open display. Run from linux desktop GUI or through Xming + PuTTy on Windows.\n", 1);
		}
	
	//get default screen (screen 1) of system
		screen = DefaultScreen(display);
	
	//define the window to open
		window = XCreateSimpleWindow(
			display,                        //display pointer
			RootWindow(display, screen),    //parent window
			10,                             //x
			10,                             //y
			1000,                            //width
			640,                            //height 20 lines @ 30px line height, 20px font height + 10 border
			0,                              //border width
			0x252525,    //border color
			0x252525     //background color
		);
	
	//ask the X server to report the events set in the x mask
		XSelectInput(display, window, ExposureMask | KeyPressMask);
	
	//map the window to the display, and use above to generate Expose events
		XMapWindow(display, window);
		
	//set graphics context and font
		font = XLoadQueryFont(display, "10x20");
		gc = XCreateGC(display, window, 0, 0);
		XSetFont(display, gc, font->fid);
	
	//auto play?
		bool autoPlaySlow = false;
		bool autoPlayFast = false;
	
	while (1) {
		
		if(autoPlaySlow || autoPlayFast){
			
			usleep(autoPlaySlow ? 200000 : 2000);
			
			//create a random keypress event and send it, so the following XNextEvent catches it
				XKeyEvent event;
				event.display     = display;
				event.window      = window;
				event.root        = RootWindow(display, screen);
				event.subwindow   = None;
				event.time        = CurrentTime;
				event.x           = 1;
				event.y           = 1;
				event.x_root      = 1;
				event.y_root      = 1;
				event.same_screen = True;
				event.keycode     = XKeysymToKeycode(display, 42);
				event.state       = 0;
				event.type = KeyPress;
				
				XSendEvent(display, window, True, KeyPressMask, (XEvent *)&event);
			
		}
	
		XNextEvent(display, &evnt);
		
		if (evnt.type == KeyPress) {
			
			if(evnt.xkey.keycode == 24){
			
				//quit "q"
					break;
					
			} else if (evnt.xkey.keycode == 27) {
			
				//restart "r"
					system_restart();
					
				//load program again
					program_load();
					
				//dissemble code
					code_disassemble();
					
				//draw current state
					draw_all(display, window, gc);
			
			} else if(evnt.xkey.keycode == 38){
				
				//autoplay toggle
					autoPlaySlow = (autoPlaySlow) ? false : true;
					autoPlayFast = false;
			
			}  else if(evnt.xkey.keycode == 39){
				
				//autoplay toggle
					autoPlayFast = (autoPlayFast) ? false : true;
					autoPlaySlow = false;
			
			} else {
			
				//progress through next cycle
					cpu_execute();
					
				//draw current state
					draw_all(display, window, gc);
					
			}
			
		} else {
		
			//all other events
				draw_all(display, window, gc);
		
		}
		
	}
	
	XCloseDisplay(display);
	
	return 0;
   
}



