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
	
	
/**
* Draw the contents of memory in a matrix of set height and width
* @return void
*/
	void draw_ram()
	{
	
		uint8_t nAddr = 0x00;
		
		//draw mem table header in cyan
			printf("%s      \t", ANSI_COLOR_CYAN);
			
			for (int col = 0; col < 16; col++){
				if(col == 8){
					printf("   ");
				}
				printf("  %01X",  col);
			}
			
			printf("%s\n",  ANSI_COLOR_RESET);
			
		//draw memory
		for (int row = 0; row < 16; row++){
		
			//split memory into two blocks
				if(row == 8){
					printf("\n");
				}
		
			char line[120] = "%s0x";
			
			char hashTemp[5];
			draw_hex((uint32_t)nAddr, 4, hashTemp);
			
			strcat(line, hashTemp);
			strcat(line, "\t%s");
			
			for (int col = 0; col < 16; col++){
			
				if(col == 8){
					strcat(line, "   ");
				}
			
				if(cpu.RAM[nAddr] == 0){
					strcat(line, " 00");
				} else {
					strcat(line, " ");
					char hashTemp2[3];
					draw_hex((uint32_t)cpu.RAM[nAddr], 2, hashTemp2);
					strcat(line, hashTemp2);
				}
				nAddr++;
			}
			
			strcat(line, "\t|");
			
			nAddr = (uint8_t)(nAddr - 0x10); //16
			for (int col = 0; col < 16; col++){
				
				if(col == 8){
					strcat(line, "| |");
				}
			
				if ((unsigned char)cpu.RAM[nAddr] >= ' ' && (unsigned char)cpu.RAM[nAddr] <= '~'){
					strcat(line, "\t|");
					char *c = " ";
					snprintf(c, 2, "%c",  cpu.RAM[nAddr]);
					strcat(line,c);
				} else {
					strcat(line,".");
				}
				
				nAddr++;
			}
			
			strcat(line,"|\n");
			
			printf(line, ANSI_COLOR_CYAN, ANSI_COLOR_RESET);
			
		}
		
	}
