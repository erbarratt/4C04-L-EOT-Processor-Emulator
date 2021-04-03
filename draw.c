#include <stdio.h>      //print, snrprintf, sprintf, fprintf
#include <string.h>     //strcat, strcmp
#include <stdlib.h>     //exit
#include <stdarg.h>     //va lists
#include "4c04.h"

char code[MEM_LENGTH][100];

/**
* Print to console
* @return void
*/
	void console_print(char * msg, int error){
	
		if(error == 1){
			fprintf(stderr, "%s", msg);
			exit(1);
		} else {
			printf("%s", msg);
		}
		
	}
	
/*
* Multiple concatenation function, with safe fill
* @param char * dest The string to concatenate into. Must be big enough to hold all strings
* @param size_t max Max number of chars to cat into destination string
* @param const char * strArgs Pointer to first variadic argument
* @param ... Variadic string arguments, must be terminated with a NULL argument
* @return char *
*/
	char * strncatcat(char * dest, size_t max, const char* strArgs, ...){
		va_list valist;
		va_start(valist, strArgs);
		const char* str = strArgs;
		while (str != NULL){
			if(strlen(dest) + strlen(str) > max){
				strncpy ( dest+strlen(dest), str, max-strlen(dest) );
				return dest;
			}
			strcat(dest, str);
	        str = va_arg(valist, const char *);
		}
	    va_end(valist);
	    return dest;
	}

/**
* Pust hex string in dest string by usage:
* char dest[size+1];
* hex((uint32_t)hex, size, dest);
* @return void
*/
	char * hex(uint32_t hex, size_t size, char * dest) {
	
		//clamp size to at least 1 char
			size = (size < 1) ? 1 : size;
			size = (size > 8) ? 8 : size;
		
		//build format string
		// "%0<size>X"
			char digits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8' };
			char formatInj[] = {digits[size], 'X'};
			char format[] = {'%', '0', formatInj[0], formatInj[1]};
		
		//create temp string and fill with blanks to intialise
			char tmp[size+1]; //max 8 chars
			for(int i = 0; i < (int)size; i++){
				tmp[i] = ' ';
			}
			
		//read the hex number into the tmp string by size+1 length in the format above
			snprintf(tmp, size+1, format, hex);
			
		//set last char as string null
			tmp[size] = '\0';
			
		//copy those bytes into the destination string
			strncpy(dest, tmp, size+1);
			
		return dest;
			
	}
	
	uint8_t find_hex_from_command(uint8_t * command){
	
		for(uint16_t k = 0; k < 64; k++){
			
			if(strcmp((char*)command, instructions[k].name) == 0){
				return (uint8_t)k;
				break;
			}
			
		}
		
		return 0;
	
	}
	
	
/**
* Draw a string to the window at a given location and color
* @param Display * display X11 display
* @param Window window X11 window
* @param GC gc X11 Graphics Context
* @param unisgned long color Hex code of colour
* @param int x X coord - text pivot is bottom left of first character
* @param int y Y coord - text pivot is bottom left of first character
* @param char * str The text to be output
* @return void
*/
	void draw_string(Display * display, Window window, GC gc, unsigned long color, int x, int y, char * str){
	
		XSetForeground(display, gc, color);
		XSetBackground(display, gc, 0x252525);
		XDrawString(display, window, gc, x, y, str, (int)strlen(str));
	
	}
	
/**
* Draw the current status of the CPU, color coded
* @param Display * display X11 display
* @param Window window X11 window
* @param GC gc X11 Graphics Context
* @return void
*/
	void draw_cpu(Display * display, Window window, GC gc){
	
		char hashTemp[5];
		char msg[50] = "CPU Status";
		int msgX = 780 + (int)(200 - (strlen(msg)*10)) / 2;
		draw_string(display, window, gc, COL_CYAN, msgX, 30, msg);
		
		//instruction txt
			strcat(strncpy(msg, "Instruction: ", 14), instructions[cpu.OPC].name);
			msgX = 780 + (int)(200 - (strlen(msg)*10)) / 2;
			draw_string(display, window, gc, ((cpu_get_drawflag(OPC) == 1) ? COL_MAGENTA : COL_WHITE), msgX, 60, msg);
		
		//PCO
			strncatcat(strncpy(msg, "PCO-[", 6), 49, hex(cpu.PCO, 2, hashTemp), "]", NULL);
			draw_string(display, window, gc, ((cpu_get_drawflag(PCO) == 1) ? COL_MAGENTA : COL_WHITE), 795, 90, msg);
			
		//OPC
			strncatcat(strncpy(msg, "OPC-[", 6), 49, hex(cpu.OPC, 2, hashTemp), "]", NULL);
			draw_string(display, window, gc, ((cpu_get_drawflag(OPC) == 1) ? COL_MAGENTA : COL_WHITE), 885, 90, msg);
			
		//STP
			strncatcat(strncpy(msg, "STP-[", 6), 49, hex(cpu.STP, 2, hashTemp), "]", NULL);
			draw_string(display, window, gc, ((cpu_get_drawflag(STP) == 1) ? COL_MAGENTA : COL_WHITE), 795, 120, msg);
			
		//CRE
			strncatcat(strncpy(msg, "CRE-[", 6), 49, hex(cpu.CRE, 2, hashTemp), "]", NULL);
			draw_string(display, window, gc, ((cpu.CRE > 0) ? COL_MAGENTA : COL_WHITE), 885, 120, msg);
			
		//IR1
			strncatcat(strncpy(msg, "IR1-[", 6), 49, hex(cpu.IR1, 2, hashTemp), "]", NULL);
			draw_string(display, window, gc, ((cpu_get_drawflag(IR1) == 1) ? COL_ORANGE : COL_WHITE), 795, 150, msg);
			
		//IR2
			strncatcat(strncpy(msg, "IR2-[", 6), 49, hex(cpu.IR2, 2, hashTemp), "]", NULL);
			draw_string(display, window, gc, ((cpu_get_drawflag(IR2) == 1) ? COL_ORANGE : COL_WHITE), 885, 150, msg);
			
		//AR0
			strncatcat(strncpy(msg, "AR0-[", 6), 49, hex(cpu.AR0, 2, hashTemp), "]", NULL);
			draw_string(display, window, gc, ((cpu_get_drawflag(AR0) == 1) ? COL_GREEN : COL_WHITE), 795, 180, msg);
			
		//AR1
			strncatcat(strncpy(msg, "AR1-[", 6), 49, hex(cpu.AR1, 2, hashTemp), "]", NULL);
			draw_string(display, window, gc, ((cpu_get_drawflag(AR1) == 1) ? COL_GREEN : COL_WHITE), 885, 180, msg);
			
		//AR2
			strncatcat(strncpy(msg, "AR2-[", 6), 49, hex(cpu.AR2, 2, hashTemp), "]", NULL);
			draw_string(display, window, gc, ((cpu_get_drawflag(AR2) == 1) ? COL_GREEN : COL_WHITE), 795, 210, msg);
			
		//AR3
			strncatcat(strncpy(msg, "AR3-[", 6), 49, hex(cpu.AR3, 2, hashTemp), "]", NULL);
			draw_string(display, window, gc, ((cpu_get_drawflag(AR3) == 1) ? COL_GREEN : COL_WHITE), 885, 210, msg);
			
		//cpu status flags
			draw_string(display, window, gc, ((cpu.C) ? COL_GREEN : COL_RED), 805, 240, ((cpu.C) ? "C:1" : "C:0"));
			draw_string(display, window, gc, ((cpu.Z) ? COL_GREEN : COL_RED), 845, 240, ((cpu.Z) ? "Z:1" : "Z:0"));
			draw_string(display, window, gc, ((cpu.V) ? COL_GREEN : COL_RED), 885, 240, ((cpu.V) ? "V:1" : "V:0"));
			draw_string(display, window, gc, ((cpu.N) ? COL_GREEN : COL_RED), 925, 240, ((cpu.N) ? "N:1" : "N:0"));
	
	}

/**
* Draw the contents of memory in a matrix of set height and width
* @param Display * display X11 display
* @param Window window X11 window
* @param GC gc X11 Graphics Context
* @return void
*/
	void draw_ram(Display * display, Window window, GC gc){
	
		uint8_t nAddr = 0x00;
		
		int y = 30; //10 + 20 height font
		int lineHeight = 30;
		int pcCol = 0, pcY = 0, pcX;
		uint8_t pcVal = 0;
		
		char txt[120];
		char hashTemp[5];
		
		//draw mem table header in cyan
			strncpy(txt, " RAM", 5);
			
			for (uint8_t col = 0; col < 16; col++){
			
				if(col == 8){
					strcat(txt, "  ");
				}
				
				if(col == 0){
					strcat(txt, " 00");
				} else {
					strncatcat(txt, 119, " ", hex(col, 2, hashTemp), NULL);
				}
				
			}
			
			strcat(txt, "      HEX > ASCII");
			draw_string(display, window, gc, COL_CYAN, 10, y, txt);
			
		//now draw bulk
			y += lineHeight;
			
			for (int row = 0; row < 16; row++){
			
				//shift stack down a bit
					if(row == 7){
						y += 15;
					}
			
				//split memory into two blocks
					if(row == 8){
					
						y += lineHeight  / 2;
						
						strncpy(txt, " ROM", 5);
			
						for (uint8_t col = 0; col < 16; col++){
						
							if(col == 8){
								strcat(txt, "  ");
							}
							
							if(col == 0){
								strcat(txt, " 00");
							} else {
								strncatcat(txt, 119, " ", hex(col, 2, hashTemp), NULL);
							}
							
						}
						
						strcat(txt, "      HEX > ASCII");
						draw_string(display, window, gc, COL_CYAN, 10, y, txt);
						
						y += lineHeight;
						
					}
			
				//draw the starting address of this line of bytes
					strcat(strncpy(txt, " $", 3), hex(nAddr, 2, hashTemp));
					draw_string(display, window, gc, COL_CYAN, 10, y, txt);
				
				//now output the line of hex bytes
					strncpy(txt, "", 2);
					
					for (int col = 0; col < 16; col++){
					
						if(col == 8){
							strcat(txt, "  ");
						}
					
						if(cpu.RAM[nAddr] == 0){
							strcat(txt, " 00");
						} else {
							strncatcat(txt, 119, " ", hex(cpu.RAM[nAddr], 2, hashTemp), NULL);
						}
						
						if(nAddr == cpu.PCO-1 && cpu.PCO-1 >= PROG_MEM_LOC){
							pcCol = col;
							pcY = y;
							pcVal = cpu.RAM[nAddr];
						}
						
						nAddr++;
					}
				
				//now convert to ascii output
					strcat(txt, "  |");
					
					nAddr = (uint8_t)(nAddr - 0x10); //16
					for (int col = 0; col < 16; col++){
						
						if(col == 8){
							strcat(txt, " ");
						}
					
						if ((unsigned char)cpu.RAM[nAddr] >= ' ' && (unsigned char)cpu.RAM[nAddr] <= '~'){
							char c[10] = " ";
							snprintf(c, 2, "%c",  cpu.RAM[nAddr]);
							strcat(txt,c);
						} else {
							strcat(txt,".");
						}
						
						nAddr++;
					}
					
					strcat(txt,"|");
				
					if(row == 7){
					
						char * msg = "<- Stack";
						int msgX = 780 + (int)(200 - (strlen(msg)*10)) / 2;
						draw_string(display, window, gc, COL_CYAN, msgX, (9*30)+15, msg);
					
					}
				
				//draw line to screen
					draw_string(display, window, gc, 0xffffff, 50, y, txt);
				
				y += lineHeight;
				
			}
			
		//draw locations AFTER the above, so it's atop it
		//PC Hex value
			pcX = 50 + (pcCol*30) + ((pcCol >= 8) ? 20 : 0);
			
			strncpy(txt, " ", 2);
			if(pcVal == 0){
				strcat(txt, "00");
			} else {
				strcat(txt, hex(pcVal, 2, hashTemp));
			}
			
			draw_string(display, window, gc, COL_ORANGE, pcX, pcY, txt);
			
		//now highlight ascii output
			pcX = 570 + (pcCol*10) + ((pcCol >= 8) ? 10 : 0);
			
			strncpy(txt, " ", 2);
			if (pcVal >= ' ' && pcVal <= '~'){
				char c[10] = " ";
				snprintf(c, 2, "%c",  pcVal);
				strcat(txt,c);
			} else {
				strcat(txt,".");
			}
			
			draw_string(display, window, gc, COL_ORANGE, pcX, pcY, txt);
		
	}
	
/**
* Draw codes lines surrounding last opcode address location
* @param Display * display X11 display
* @param Window window X11 window
* @param GC gc X11 Graphics Context
* @return void
*/
	void draw_code(Display * display, Window window, GC gc, uint8_t addr){
	
		char * msg = "Code Lines";
		int msgX = 780 + (int)(200 - (strlen(msg)*10)) / 2;
		draw_string(display, window, gc, COL_CYAN, msgX, 330, msg);
	
		char lines[8][100];
		
		int lower = 0, count = 0, nullLines = 0;
		uint16_t start;
		uint16_t lastOpAddrFound = 0;
		start = addr;
		
		//midpoint
			uint8_t midPoint = 3;
		
		//go backwards finding the 3 instructions before the current.
		//in the dissassembler, inter-instruction memorylocations are padded with ---
		//these are ignored as aren't instructions
		//programs are always surrounded with NOP commands from memory + code[] init
			while(lower < midPoint){
				
				start--;
				
				if(strcmp(code[start], "---") != 0){
					lower++;
					lastOpAddrFound = start;
					//reset null lines so we don't count 6 overall
					nullLines = 0;
				} else {
				
					nullLines++;
					if(nullLines == 6){
						start = lastOpAddrFound;
						
						for(int i = 0; i <= midPoint-lower; i++){
							strcpy(lines[i], "---");
						}
						
						count = midPoint-lower;
						lower = midPoint;
					}
					
				}
			
			}
			
		//now move forward from this position, ignore --- and store the op line
			while(count < 8){
			
				if(strcmp(code[start], "---") != 0){
					strcpy(lines[count], code[start]);
					count++;
				}
				
				start++;
			
			}
		
		//finally now we have full lines, output them highlighting current instruction
			for(int i = 0; i < 8; i++){
			
				int lineX = 780 + (int)(200 - (strlen(lines[i])*10)) / 2;
				int lineY = (12*30) + (i*30);
			
				if(i == midPoint){
					//draw cyan
					draw_string(display, window, gc, COL_CYAN, lineX, lineY, lines[i]);
				} else {
					//draw white
					draw_string(display, window, gc, COL_WHITE, lineX, lineY, lines[i]);
				}
			}
	
	}

/**
* Draw all elements of the program
* @param Display * display X11 display
* @param Window window X11 window
* @param GC gc X11 Graphics Context
* @return void
*/
	void draw_all(Display * display, Window window, GC gc){
	
		XClearWindow(display, window);
		draw_ram(display, window, gc);
		draw_cpu(display, window, gc);
		draw_code(display, window, gc, cpu.lastOpAddr);
		
		char * msg = "Press Space to step through CPU Cycles. Q = Quit, R = Reset, A = Slow Auto, F = Fast Auto.";
		int msgX = (int)(1000 - (strlen(msg)*10)) / 2;
		draw_string(display, window, gc, COL_WHITE, msgX, 615, msg);
	
	}
	
/**
* Dissassemble the memory program into coded lines to display
* @return void
*/
	void code_disassemble(){
	
		char line [50] = "";
		char num[5];
		char cycles[10];
	
		for(uint8_t i = 0; i < 128; i++){
			strncpy(code[i], strncatcat(strcpy(line, "$"), 49, hex(i, 2, num),": NOP c2", NULL), 12);
		}
	
		//start at program address
			uint16_t addr = PROG_MEM_LOC;
			uint16_t lineAddr;
		
		while(addr < MEM_LENGTH){
		
			//start line string
				lineAddr = addr;
				uint8_t opcode = cpu.RAM[addr];
				snprintf(cycles, 4,"%d", instructions[opcode].cycles);
				strncatcat(strcpy(line, "$"), 49, hex(addr, 2, num),": ",instructions[opcode].name," c",cycles, NULL);
				
			//get next addr
				addr++;
			
			//use instructions array to fill in the mem locations where data and not opcodes are
				for(int i = 0; i < instructions[opcode].pcoShifts; i++){
				
					strcpy(code[addr], "---");
					addr++;
				
				}
			
			//write that line to code array
				strcpy(code[lineAddr], line);
		
		}
		
		for(uint16_t i = PROG_MEM_LOC; i < MEM_LENGTH; i++){
			//	printf("%s\n", code[i]);
		}
	
	}

/**
* See if char is valid hex char to convert
* @param c
* @return bool
*/
	bool program_is_hex_char(uint8_t c){
	
		if (
			(c >= '0' && c <= '9') ||
			(c >= 'a' && c <= 'f') ||
			(c >= 'A' && c <= 'F')
		){
			return true;
		}
		
		return false;
		
	}
	
/**
* Get the hex value represented by the ascii char
* @param c
* @return uint8_t
*/
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
				console_print("No Assembly File 'prog.txt'. Closing...\n", true);
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

		//process file
			int i = 1;
			while(1){
				
				//get next char
					c = (uint8_t)fgetc(fp);
					//printf("%c", c);
		
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
							//printf("%c", cHere);
							
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
							//printf("%c", cHere);
							
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
								//printf("%c", cHere);
								
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
								//printf("%c", cHere);
								
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
										//printf("%c", cHere);
										
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
									
									uint8_t commandHere[4] = "JMP";
									cpu.RAM[addr] = find_hex_from_command(commandHere);
									addr++;
									cpu.RAM[addr] = 0x00; //00 for now
									addr++;
									
								//read to next newline
									if(hitNewLine == false){
										while(true){
										
											char cHere = (char)fgetc(fp);
											//printf("%c", cHere);
											
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
										//printf("%c", cHere);
										
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
										
										subLabel[j] = cHere;
									
									}
									
									subLabel[j] = '\0';
								
								//now go through subroutines array to find match
									for(j = 0; j < MEM_LENGTH; j++){
									
										if(strcmp(subroutines[j], subLabel) == 0){
											
											//we've found a match, to J represents the start of that subroutine
											//that will always be 12 XX, so we need to set JSR value to two beyond that
												uint8_t commandHere[4] = "JSR";
												cpu.RAM[addr] = find_hex_from_command(commandHere); //JSR
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
											//printf("%c", cHere);
											
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
										//printf("%c", cHere);
										
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
										
										markers[addr][j] = cHere;
									
									}
									
									markers[addr][j] = '\0';
									
								//markers are just places in memory, and are ignored at execution
								//they are always a JMP to just after themselves
									//cpu.RAM[addr] = 0x12; //JMP
									//addr++;
									//cpu.RAM[addr] = (uint8_t)(addr+1);
									//addr++;
									
								//read to next newline
									if(hitNewLine == false){
										while(true){
										
											char cHere = (char)fgetc(fp);
											//printf("%c", cHere);
											
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
									cpu.RAM[addr] = find_hex_from_command(command);
								
								//RFS means we can close current subroutine count and set where to jump now
									if (strcmp((char*)command, "RFS") == 0){
										
										if(inSubroutineDef){
											cpu.RAM[lastSubroutineAddr+1] = (uint8_t)(lastSubroutineAddr+3+subroutineLocCount);
											subroutineLocCount = 0;
											inSubroutineDef = false;
										}
										
									}
							
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
										//printf("%c", cHere);
										
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
												cpu.RAM[addr] = (uint8_t)(j);
												addr++;
											
											break;
											
										}
									
									}
									
								//read to next newline
									if(hitNewLine == false){
										while(true){
										
											char cHere = (char)fgetc(fp);
											//printf("%c", cHere);
											
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
							
								//reset i and flag so character after this continues to be parsed as normal
									i = 1;
									characterLiteral = false;
							
								//store the value as the character ascii binary
									cpu.RAM[addr] = c;
									addr++;
									bytes++;
									if(inSubroutineDef){
										subroutineLocCount++;
									}
						
						//get the hex val and store as byte is on lo pass
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
			
		//close file
			fclose(fp);
	
	}
