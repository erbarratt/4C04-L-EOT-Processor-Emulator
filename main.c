#include "main.h"

int main(void){

	printf("4c04 L EOT CPU Emulator\n");
	printf("System Booting...\n");

	system_restart();
	
	sleep(1);
	
	program_load();
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

void program_load(){

	//assembly rules:
	/*
	* Assembly rules:
	*
	* lines should never start with white space
	*
	* comments are started with the ; character and always stopped with a new line
	*
	* commands are always 3 letters words "NPG"
	* command lines must start with a command word followed by spaces and 2 digit hex values as arguments (oprands)
	* 1 command per line
	*
	* Subroutines must be defined before calling
	* Subroutines are derfined as starting a new line with the character '@' followed by up to 10 chars, no spaces
	* Subroutines are called by starting a new line with > and then typing the matching subroutine name, no spaces
	*
	*/

	//load program into program memory
		FILE * fp;
		
		fp = fopen("prog.txt", "r");
		
		if (fp == NULL){
			printf("No Assembly File\n");
			exit(1);
		}
	
	//holding vars
	
		//a subroutine could be defined at an point in memory
			char subroutines[MEM_LENGTH][12];
			uint8_t lastSubroutineAddr = 0;
			uint8_t subroutineLocCount = 0;
			bool inSubroutine = false;
		
		uint8_t command[4] = "NOP";
		int col = 0;
		uint8_t c, new, tmp;
		uint8_t hi = 0x00;
		uint8_t lo;
		
		int bytes = 0;
		uint8_t addr = PROG_MEM_LOC;
		
		bool comment = false;
		
		bool dataMode = false;
		
		int i = 1;
		while(1){
			
			//get next char
				c = (uint8_t)fgetc(fp);
	
			//check if end of file or end of memory now
				if(feof(fp) || bytes > 128){
					break;
				}
				
			//see if we got comment
				if(c == ';'){
					comment = true;
					continue;
				}
				
				if(comment && c != '\n'){
					continue;
				}
			
			//new line starts the command
				if(c == '\n'){
					comment = false;
					col = 0;
					continue;
				}
				
			//see if we're in data mode
				if(col == 0 && c == '#'){
				
					dataMode = true;
					
					//read to next newline
						for(int j = 0; j < 10; j++){
						
							char cHere = (char)fgetc(fp);
							
							if(cHere == '\n'){
								break;
							}
						
						}
					
					continue;
				}
				
			//see if we're in command mode
				if(col == 0 && c == '!'){
					
					dataMode = false;
					
					//read to next newline
						for(int j = 0; j < 10; j++){
						
							char cHere = (char)fgetc(fp);
							
							if(cHere == '\n'){
								break;
							}
						
						}
					
					continue;
					
				}
				
			//if col index 0 - 3, then we're looking to build command
				if(col < 3 && !dataMode){
				
					//subroutine defintion
						if(col == 0 && c == '@'){
						
							for(int j = 0; j < 10; j++){
							
								char cHere = (char)fgetc(fp);
								
								if(cHere == '\n'){
									break;
								}
								
								subroutines[addr][j] = cHere;
							
							}
							
							printf("%s\n", subroutines[addr]);
							printf("%X\n", addr);
						
							//@subroutine need replacing with
							//	JMP XX ;location after subroutine so it's ignored during execution
							//
							
							inSubroutine = true;
							lastSubroutineAddr = addr;
							
							cpu.RAM[addr] = 0x12; //JMP
							addr++;
							cpu.RAM[addr] = 0x00; //00 for now
							addr++;
							continue;
						
						}
						
					//subroutine replacement
						else if(col == 0 && c == '>'){
						
							char subLabel[12];
							
							for(int j = 0; j < 10; j++){
							
								char cHere = (char)fgetc(fp);
								
								if(cHere == '\n'){
									break;
								}
								
								subLabel[j] = cHere;
							
							}
							
							printf("%s\n", subLabel);
							
							//now go through subroutines array to find match
								for(uint16_t j = 0; j < MEM_LENGTH; j++){
								
									if(strcmp(subroutines[j], subLabel) == 0){
									
										cpu.RAM[addr] = 0x15; //JSR
										addr++;
										cpu.RAM[addr] = (uint8_t)(j+2);
										addr++;
										break;
										
									}
								
								}
							
							continue;
						
						}
				
					//command line
						command[col] = c;
						
						if(col == 2){
						
							//find instruction and write as it's hex equivalent
								if(strcmp((char*)command, "NOP") == 0){ cpu.RAM[addr] = 0x00; }
								
								else if (strcmp((char*)command, "LRV") == 0){ cpu.RAM[addr] = 0x01; }
								else if (strcmp((char*)command, "LRM") == 0){ cpu.RAM[addr] = 0x02; }
								else if (strcmp((char*)command, "LRR") == 0){ cpu.RAM[addr] = 0x03; }
								
								else if (strcmp((char*)command, "NOP") == 0){ cpu.RAM[addr] = 0x04; }
								else if (strcmp((char*)command, "NOP") == 0){ cpu.RAM[addr] = 0x05; }
								
								else if (strcmp((char*)command, "STV") == 0){ cpu.RAM[addr] = 0x06; }
								else if (strcmp((char*)command, "STR") == 0){ cpu.RAM[addr] = 0x07; }
								
								else if (strcmp((char*)command, "NOP") == 0){ cpu.RAM[addr] = 0x08; }
								else if (strcmp((char*)command, "NOP") == 0){ cpu.RAM[addr] = 0x09; }
								
								else if (strcmp((char*)command, "BMP") == 0){ cpu.RAM[addr] = 0x0a; }
								else if (strcmp((char*)command, "SQR") == 0){ cpu.RAM[addr] = 0x0b; }
								
								else if (strcmp((char*)command, "NOP") == 0){ cpu.RAM[addr] = 0x0c; }
								else if (strcmp((char*)command, "NOP") == 0){ cpu.RAM[addr] = 0x0d; }
								
								else if (strcmp((char*)command, "ADD") == 0){ cpu.RAM[addr] = 0x0e; }
								else if (strcmp((char*)command, "SUB") == 0){ cpu.RAM[addr] = 0x0f; }
								
								else if (strcmp((char*)command, "NOP") == 0){ cpu.RAM[addr] = 0x10; }
								else if (strcmp((char*)command, "NOP") == 0){ cpu.RAM[addr] = 0x11; }
								
								else if (strcmp((char*)command, "JMP") == 0){ cpu.RAM[addr] = 0x12; }
								else if (strcmp((char*)command, "JNE") == 0){ cpu.RAM[addr] = 0x13; }
								else if (strcmp((char*)command, "JZS") == 0){ cpu.RAM[addr] = 0x14; }
								else if (strcmp((char*)command, "JSR") == 0){ cpu.RAM[addr] = 0x15; }
								else if (strcmp((char*)command, "RFS") == 0){
								
									cpu.RAM[addr] = 0x16;
									if(inSubroutine){
										cpu.RAM[lastSubroutineAddr+1] = lastSubroutineAddr+3+subroutineLocCount;
										subroutineLocCount = 0;
										inSubroutine = false;
									}
									
								}
						
							addr++;
							bytes++;
							if(inSubroutine){
								subroutineLocCount++;
							}
						
						}
						
						col++;
				
			//otherwise, we're always reading hex pairs
				} else {
		
					//check this char is in valid hex char set
					if(
						c != 'a' &&
						c != 'A' &&
						c != 'b' &&
						c != 'B' &&
						c != 'c' &&
						c != 'C' &&
						c != 'd' &&
						c != 'D' &&
						c != 'e' &&
						c != 'E' &&
						c != 'f' &&
						c != 'F' &&
						c != '0' &&
						c != '1' &&
						c != '2' &&
						c != '3' &&
						c != '4' &&
						c != '5' &&
						c != '6' &&
						c != '7' &&
						c != '8' &&
						c != '9'
					){
						continue;
					}
			
					//lookup 8 bit value of hex character to store in tmp
						switch(c){
							case '1': tmp = 0x01; break;
							case '2': tmp = 0x02; break;
							case '3': tmp = 0x03; break;
							case '4': tmp = 0x04; break;
							case '5': tmp = 0x05; break;
							case '6': tmp = 0x06; break;
							case '7': tmp = 0x07; break;
							case '8': tmp = 0x08; break;
							case '9': tmp = 0x09; break;
							case 'a': case 'A': tmp = 0x0A; break;
							case 'b': case 'B': tmp = 0x0B; break;
							case 'c': case 'C': tmp = 0x0C; break;
							case 'd': case 'D': tmp = 0x0D; break;
							case 'e': case 'E': tmp = 0x0E; break;
							case 'f': case 'F': tmp = 0x0F; break;
							default: tmp = 0x00; break;
						}
			
					//write only when hex built from both chars in txt file
						if(i == 1){
							i++;
							hi = tmp;
						} else {
							lo = tmp;
							i = 1;
							
							//new is hi bit shifted left by four, bitwise OR's with lo
							//since lo always has 4 leading zeros, the hi 1 bits are always written (one OR the other bit is 1...)
							new = (uint8_t)(hi << 4) | lo;
							cpu.RAM[addr] = new;
							addr++;
							bytes++;
							if(inSubroutine){
								subroutineLocCount++;
							}
						}
						
				}
	
		}
		
		fclose(fp);

}

