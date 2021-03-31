#include "main.h"

int main(void){

	printf("4c04 L EOT CPU Emulator\n");
	printf("System Booting...\n");

	system_restart();
	
	sleep(1);
	
	printf("Program Loaded...\n");
	
	code_disassemble();
	
	usleep(200000);
	
	printf("Opening Window...\n");
	
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
	
		if(autoPlaySlow){
			
			usleep(200000); // 0.5s
			
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
			
		} else if (autoPlayFast){
		
			usleep(2000); // 0.5s
			
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
