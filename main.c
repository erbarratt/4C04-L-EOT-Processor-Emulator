#include "main.h"

int main(void){

	system_restart();
	
	sleep(1);
	
	printf("system restarted \n");
	
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
			fprintf(stderr, "Cannot open display. Run from linux desktop GUI or through Xming + putty on Windows.\n");
			exit(1);
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
			610,                            //height 20 lines @ 30px line height, 20px font height + 10 border
			0,                              //border width
			0x252525,    //border color
			0x252525     //background color
		);
	
	//ask the X server to report the events set in the x mask
		XSelectInput(display, window, ExposureMask | KeyPressMask);
	
	//map the window to the display, and use above to generate Expose events
		XMapWindow(display, window);
		
	//set graphics context and font
		//XFontStruct* myFont = XLoadQueryFont(display, "-Misc-Fixed-Medium-R-Normal--7-70-75-75-C-50-ISO10646-1");
		font = XLoadQueryFont(display, "10x20");
		gc = XCreateGC(display, window, 0, 0);
		XSetFont(display, gc, font->fid);
	
	while (1) {
	
		XNextEvent(display, &evnt);
		
		//if (evnt.type == Expose) {
		//	//XFillRectangle(display, window, DefaultGC(display, screen), 20, 20, 10, 10);
		//
		//}
		
		if (evnt.type == KeyPress) {
			
			if(evnt.xkey.keycode == 24){
			
				//quit "q"
					break;
					
			} else if (evnt.xkey.keycode == 27) {
			
				//restart "r"
					system_restart();
					
				//draw current state
					draw_all(display, window, gc);
			
			} else {
			
				//progress through next cycle
					cpu_execute();
					
					printf("%#X %d\n", cpu.PCO, cpu.CRE);
					
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
