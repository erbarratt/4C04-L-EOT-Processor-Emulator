#include "main.h"

int main(void){

	printf("4c04 L EOT CPU Emulator\n");
	printf("System Booting...\n");

	system_restart();
	
	//sleep(1);
	
	program_load();
	printf("Program Loaded...\n");
	
	code_disassemble();
	
	//usleep(200000);
	
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

	bool program_is_hex_char(uint8_t c){
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
			return false;
		}
		
		return true;
	}
	
	uint8_t program_get_hex_char(uint8_t c){
	
		//lookup 8 bit value of hex character to store in tmp
			switch(c){
				case '1': return 0x01;
				case '2': return 0x02;
				case '3': return 0x03;
				case '4': return 0x04;
				case '5': return 0x05;
				case '6': return 0x06;
				case '7': return 0x07;
				case '8': return 0x08;
				case '9': return 0x09;
				case 'a': case 'A': return 0x0A;
				case 'b': case 'B': return 0x0B;
				case 'c': case 'C': return 0x0C;
				case 'd': case 'D': return 0x0D;
				case 'e': case 'E': return 0x0E;
				case 'f': case 'F': return 0x0F;
				default: return 0x00;
			}
	
	}

/**
* Load a program from a text file called prog.txt
* @return void
*/
	void program_load(){
	
		/*
		* Assembly rules:
		*
		* lines should never start with white space
		*
		* comments are started with the ';' character and always stopped with a new line
		*
		* commands are always 3 letters words "NPG"
		* command lines must start with a command word followed by spaces and 2 digit hex values or register pointers as arguments (oprands)
		* 1 command per line
		*
		* Subroutines must be defined before calling
		* Subroutines are derfined as starting a new line with the character '@' followed by up to 10 chars, no spaces
		* Subroutines are called by starting a new line with '>' and then typing the matching subroutine name, no spaces
		*
		* Addressable Registers can be defined using RX, where X is 0 - 3, insted of the hex equivalent
		* '$' characters are ignored, and so can be used to define memory locations
		*
		* ':' characters denote a memory marker, useful for literal jumps to that position
		* Markers can contain no spaces and are up to 10 characters
		* Use a marker by the character '-' followed by the marker name defined above
		*
		* ^ followed by a hex number pair will move the address of where you're writing memory to to that location
		*
		* # at the start of a line puts the assembler in data mode, where it will sequentially write the bytes defined after it
		* ! at the start of a line puts the assembler in command mode, where it will parse commands as normal
		*
		*/
	
		//load program into program memory
			FILE * fp;
			
			fp = fopen("prog.txt", "r");
			
			if (fp == NULL){
				printf("No Assembly File 'prog.txt'. Closing...\n");
				exit(1);
			}
		
		//holding vars
		
			//a subroutine could be defined at an point in memory
				char subroutines[MEM_LENGTH][12];
				uint8_t lastSubroutineAddr = 0;
				uint8_t subroutineLocCount = 0;
				bool inSubroutineDef = false;
				
			//markers
				char markers[MEM_LENGTH][12];
			
			uint8_t command[4] = "NOP";
			int col = 0;
			uint8_t c, new, tmp;
			uint8_t hi = 0x00;
			uint8_t lo;
			
			int bytes = 0;
			uint8_t addr = PROG_MEM_LOC;
			
			bool comment = false;
			bool dataMode = false;
			bool characterLiteral = false;
			
			int i = 1;
			while(1){
				
				//get next char
					c = (uint8_t)fgetc(fp);
					printf("%c", c);
		
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
					
				//jump to mem loc
					if(col == 0 && c == '^'){
					
						//get following two bytes and transform
						bool hiSet = false;
						while(1){
						
							uint8_t cHere = (uint8_t)fgetc(fp);
							printf("%c", cHere);
							
							if(program_is_hex_char(cHere) == false){
								continue;
							}
							
							if(hiSet == false){
								hi = program_get_hex_char(cHere);
								hiSet = true;
							} else {
								lo = program_get_hex_char(cHere);
								addr =  (uint8_t)(hi << 4) | lo;
								break;
							}
						
						}
						
						while(true){
						
							char cHere = (char)fgetc(fp);
							printf("%c", cHere);
							
							if(cHere == '\n'){
								break;
							}
						
						}
						
						continue;
					
					}
					
				//see if we're in data mode
					if(col == 0 && c == '#'){
					
						dataMode = true;
						
						//read to next newline
							while(true){
							
								char cHere = (char)fgetc(fp);
								printf("%c", cHere);
								
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
								printf("%c", cHere);
								
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
							
								//define the label, only take in the first 10 chars
									int j;
									bool hitNewLine = false;
									for(j = 0; j < 10; j++){
									
										char cHere = (char)fgetc(fp);
										printf("%c", cHere);
										
										if(
											cHere == '\n'
										){
											hitNewLine = true;
											break;
										}
										
										if(
											cHere == ' ' ||
											cHere == '\t'
										){
											break;
										}
										
										subroutines[addr][j] = cHere;
									
									}
									
									subroutines[addr][j] = '\0';
									
								//set as now being in a subroutine for the following instructions
								//to be able to add them to count of subroutine instructions
								//this count is then used to determine the length of the subroutine
								//so it can be retrospectively put as the jump value when we hit the RFS instruction
								
									inSubroutineDef = true;
									lastSubroutineAddr = addr;
									
									cpu.RAM[addr] = 0x12; //JMP
									addr++;
									cpu.RAM[addr] = 0x00; //00 for now
									addr++;
									
								//read to next newline
									if(hitNewLine == false){
										while(true){
										
											char cHere = (char)fgetc(fp);
											printf("%c", cHere);
											
											if(cHere == '\n'){
												break;
											}
										
										}
									}
									
								//reset col so next line works correctly
									comment = false;
									col = 0;
									
								continue;
							
							}
							
						//subroutine replacement
							else if(col == 0 && c == '>'){
							
								//construct label to match in the subroutine array
									char subLabel[12];
									
									int j;
									bool hitNewLine = false;
									for(j = 0; j < 10; j++){
									
										char cHere = (char)fgetc(fp);
										printf("%c", cHere);
										
										if(cHere == '\n'){
											break;
										}
										
										if(
											cHere == ' ' ||
											cHere == '\t'
										){
											break;
										}
										
										subLabel[j] = cHere;
									
									}
									
									subLabel[j] = '\0';
								
								//now go through subroutines array to find match
									for(j = 0; j < MEM_LENGTH; j++){
									
										if(strcmp(subroutines[j], subLabel) == 0){
											
											//we've found a match, to J represents the start of that subroutine
											//that will always be 12 XX, so we need to set JSR value to two beyond that
												cpu.RAM[addr] = 0x15; //JSR
												addr++;
												cpu.RAM[addr] = (uint8_t)(j+2);
												addr++;
											
											//we might be in a subroutine definition, so we need to include these two bytes in that count too
												if(inSubroutineDef){
													subroutineLocCount++;
													subroutineLocCount++;
												}
											
											break;
											
										}
									
									}
									
								//read to next newline
									if(hitNewLine == false){
										while(true){
										
											char cHere = (char)fgetc(fp);
											printf("%c", cHere);
											
											if(cHere == '\n'){
												break;
											}
										
										}
									}
									
								//reset col so next line works correctly
									comment = false;
									col = 0;
								
								continue;
							
							}
							
						//markers
							else if(col == 0 && c == ':'){
							
								//define the label, only take in the first 10 chars
									int j;
									bool hitNewLine = false;
									for(j = 0; j < 10; j++){
									
										char cHere = (char)fgetc(fp);
										printf("%c", cHere);
										
										if(cHere == '\n'){
											break;
										}
										
										if(
											cHere == ' ' ||
											cHere == '\t'
										){
											break;
										}
										
										markers[addr][j] = cHere;
									
									}
									
									markers[addr][j] = '\0';
									
								//markers are just places in memory, and are ignored at execution
								//they are always a JMP to just after themselves
									cpu.RAM[addr] = 0x12; //JMP
									addr++;
									cpu.RAM[addr] = (uint8_t)(addr+1);
									addr++;
									
								//read to next newline
									if(hitNewLine == false){
										while(true){
										
											char cHere = (char)fgetc(fp);
											printf("%c", cHere);
											
											if(cHere == '\n'){
												break;
											}
										
										}
									}
									
								//reset col so next line works correctly
									comment = false;
									col = 0;
									
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
										if(inSubroutineDef){
											cpu.RAM[lastSubroutineAddr+1] = (uint8_t)(lastSubroutineAddr+3+subroutineLocCount);
											subroutineLocCount = 0;
											inSubroutineDef = false;
										}
										
									}
									
									else if (strcmp((char*)command, "NOP") == 0){ cpu.RAM[addr] = 0x17; }
									else if (strcmp((char*)command, "NOP") == 0){ cpu.RAM[addr] = 0x18; }
									
									else if (strcmp((char*)command, "AND") == 0){ cpu.RAM[addr] = 0x19; }
									else if (strcmp((char*)command, "NOP") == 0){ cpu.RAM[addr] = 0x1a; }
							
								addr++;
								bytes++;
								if(inSubroutineDef){
									subroutineLocCount++;
								}
							
							}
							
							col++;
					
				//otherwise, we're always reading hex pairs
					} else {
					
						//set character literal mode
							if(i == 1 && c == '"'){
								characterLiteral = true;
								i++;
								continue;
							}
						
						//replace "R" with 0, making code easier to read
							if(i== 1 && c == 'R'){
								hi = 0x00;
								i++;
								continue;
							}
							
						//raplce markers with addr
							if(i == 1 && c == '-'){
							
								//construct label to match in the subroutine array
									char markerLabel[12];
									bool hitNewLine = false;
									
									int j;
									for(j = 0; j < 10; j++){
									
										char cHere = (char)fgetc(fp);
										printf("%c", cHere);
										
										if(cHere == '\n'){
											hitNewLine = true;
											break;
										}
										
										if(
											cHere == ' ' ||
											cHere == '\t'
										){
											break;
										}
										
										markerLabel[j] = cHere;
									
									}
									
									markerLabel[j] = '\0';
								
								//now go through subroutines array to find match
									for(j = 0; j < MEM_LENGTH; j++){
									
										if(strcmp(markers[j], markerLabel) == 0){
											
											//we've found a match, to J represents the start of that subroutine
											//that will always be 12 XX, so we need to set JSR value to two beyond that
												cpu.RAM[addr] = (uint8_t)(j+2);
												addr++;
											
											break;
											
										}
									
									}
									
								//read to next newline
									if(hitNewLine == false){
										while(true){
										
											char cHere = (char)fgetc(fp);
											printf("%c", cHere);
											
											if(cHere == '\n'){
												break;
											}
										
										}
									}
									
								//reset col so next line works correctly
									comment = false;
									col = 0;
								
								continue;
							
							}
						
						//following char is taken as the value, no matter what
						if(characterLiteral){
						
							i = 1;
							characterLiteral = false;
						
							//store the value as the character ascii binary
								cpu.RAM[addr] = c;
								addr++;
								bytes++;
								if(inSubroutineDef){
									subroutineLocCount++;
								}
						
						} else {
			
							//check this char is in valid hex char set
								if(program_is_hex_char(c) == false){
									continue;
								}
								
								tmp = (uint8_t)program_get_hex_char(c);
					
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
									if(inSubroutineDef){
										subroutineLocCount++;
									}
								}
							
						}
						
					}
		
			}
			
			fclose(fp);
	
	}

