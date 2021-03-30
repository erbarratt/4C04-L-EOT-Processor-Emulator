#include "draw.h"
#include "cpu.h"

//colors for printing
	char ANSI_COLOR_RED[] = "\x1b[31m";
	char ANSI_COLOR_GREEN[] = "\x1b[32m";
	char ANSI_COLOR_YELLOW[] = "\x1b[33m";
	char ANSI_COLOR_BLUE[] = "\x1b[34m";
	char ANSI_COLOR_MAGENTA[] = "\x1b[35m";
	char ANSI_COLOR_CYAN[] = "\x1b[36m";
	char ANSI_COLOR_RESET[] = "\x1b[0m";
	
/**
* Pust hex string in dest string by usage:
* char dest[size+1];
* draw_hex((uint32_t)hex, size, dest);
* @return void
*/
	void draw_hex(uint32_t hex, size_t size, char * dest) {
	
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
			
	}
	
	void draw_string(Display * display, Window window, GC gc, unsigned long color, int x, int y, char * str){
	
		XSetForeground(display, gc, color);
		XSetBackground(display, gc, 0x252525);
		XDrawString(display, window, gc, x, y, str, (int)strlen(str));
	
	}
	
/**
* Draw the contents of memory in a matrix of set height and width
* @return void
*/
	void draw_ram(Display * display, Window window, GC gc)
	{
	
		uint8_t nAddr = 0x00;
		
		int y = 30; //10 + 20 height font
		int lineHeight = 30;
		int pcCol = 0, pcY = 0, pcX = 0;
		uint8_t pcVal = 0;
		
		//draw mem table header in cyan
			char topLine[120] = "      ";
			
			for (int col = 0; col < 16; col++){
			
				if(col == 8){
					strcat(topLine, "  ");
				}
				
				if(col == 0){
					strcat(topLine, " 00");
				} else {
					strcat(topLine, " ");
					char hashTemp2[5];
					draw_hex((uint32_t)col, 2, hashTemp2);
					strcat(topLine, hashTemp2);
				}
				
			}
			
			draw_string(display, window, gc, 0x28d9ed, 10, y, topLine);
			
		//now draw bulk
			y += lineHeight;
			
			for (int row = 0; row < 16; row++){
			
				//split memory into two blocks
					if(row == 8){
						y += 30;
					}
			
				//draw the starting address of this line of bytes
					char memStart[8] = "0x";
					char hashTemp[5];
					draw_hex((uint32_t)nAddr, 4, hashTemp);
					strcat(memStart, hashTemp);
					draw_string(display, window, gc, 0x28d9ed, 10, y, memStart);
				
				//now output the line of hex bytes
					char line[120] = "";
					
					for (int col = 0; col < 16; col++){
					
						if(col == 8){
							strcat(line, "  ");
						}
					
						if(cpu.RAM[nAddr] == 0){
							strcat(line, " 00");
						} else {
							strcat(line, " ");
							char hashTemp2[4];
							draw_hex(cpu.RAM[nAddr], 2, hashTemp2);
							strcat(line, hashTemp2);
						}
						
						if(nAddr == cpu.PC-1 && cpu.PC-1 >= PROG_MEM_LOC){
							pcCol = col;
							pcY = y;
							pcVal = cpu.RAM[nAddr];
						}
						
						nAddr++;
					}
				
				//now convert to ascii output
					strcat(line, "  |");
					
					nAddr = (uint8_t)(nAddr - 0x10); //16
					for (int col = 0; col < 16; col++){
						
						if(col == 8){
							strcat(line, " ");
						}
					
						if ((unsigned char)cpu.RAM[nAddr] >= ' ' && (unsigned char)cpu.RAM[nAddr] <= '~'){
							char c[10] = " ";
							snprintf(c, 2, "%c",  cpu.RAM[nAddr]);
							strcat(line,c);
						} else {
							strcat(line,".");
						}
						
						nAddr++;
					}
					
					strcat(line,"|");
				
				//draw line to screen
					draw_string(display, window, gc, 0xffffff, 70, y, line);
				
				y += 30;
				
			}
			
		//draw locations AFTER the above, so it's atop it
			if(cpu.PC-1 >= PROG_MEM_LOC){
		
				//PC Hex value
					pcX = 70 + (pcCol*30) + ((pcCol >= 8) ? 20 : 0);
					
					char pcLine[50] = " ";
					
					if(pcVal == 0){
						strcat(pcLine, "00");
					} else {
						char hashTemp2[4];
						draw_hex(pcVal, 2, hashTemp2);
						strcat(pcLine, hashTemp2);
					}
					
					draw_string(display, window, gc, 0xff5100, pcX, pcY, pcLine);
					
				//now highlight ascii output
					pcX = 590 + (pcCol*10) + ((pcCol >= 8) ? 10 : 0);
					
					char asciiLine[50] = " ";
					if (pcVal >= ' ' && pcVal <= '~'){
						char c[10] = " ";
						snprintf(c, 2, "%c",  pcVal);
						strcat(asciiLine,c);
					} else {
						strcat(asciiLine,".");
					}
					
					draw_string(display, window, gc, 0xff5100, pcX, pcY, asciiLine);
					
			}
		
	}

	void draw_all(Display * display, Window window, GC gc){
	
		XClearWindow(display, window);
		draw_ram(display, window, gc);
		
		char * msg = "Press Space to sep through CPU Cycles. Press Q to quit or R to reset system.";
		
		int msgX = (int)(1000 - (strlen(msg)*10)) / 2;
	
		draw_string(display, window, gc, 0xFFFFFF, msgX, 600, msg);
	
	}
	
