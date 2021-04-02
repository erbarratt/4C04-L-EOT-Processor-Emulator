#include <string.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include "4c04.h"

char code[MEM_LENGTH][100];
	
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
	
		char * msg = "CPU Status";
		int msgX = 780 + (int)(200 - (strlen(msg)*10)) / 2;
		draw_string(display, window, gc, 0x28d9ed, msgX, 30, msg);
		
		//instruction name - now seen in code output
		//	char arMsg3[100] = "Instruction: ";
		//	strcat(arMsg3, instructions[cpu.OPC].name);
		//	int arMsg3X = 780 + (int)(200 - (strlen(arMsg3)*10)) / 2;
			//draw_string(display, window, gc, 0xFFFFFF, arMsg3X, 60, arMsg3);
		
		//PCO and OPcode
			char pcMsg[50] = "PCO-[";
			char hashTemp[5];
			draw_hex((uint32_t)cpu.PCO, 2, hashTemp);
			strcat(pcMsg, hashTemp);
			
			strcat(pcMsg, "] OPC-[");
			char hashTemp2[5];
			draw_hex((uint32_t)cpu.OPC, 2, hashTemp2);
			strcat(pcMsg, hashTemp2);
			strcat(pcMsg, "]");
			
			int pcMsgX = 780 + (int)(200 - (strlen(pcMsg)*10)) / 2; //805
			draw_string(display, window, gc, 0xFFFFFF, pcMsgX, 90, pcMsg);
		
		//Stack Pointer and Cycles Remaining
			char adMsg[50] = "STP-[";
			char hashTemp3[5];
			draw_hex((uint32_t)cpu.STP, 2, hashTemp3);
			strcat(adMsg, hashTemp3);
			
			strcat(adMsg, "] CRE-[");
			char hashTemp4[5];
			draw_hex((uint32_t)cpu.CRE, 2, hashTemp4);
			strcat(adMsg, hashTemp4);
			strcat(adMsg, "]");
			
			int adMsgX = 780 + (int)(200 - (strlen(adMsg)*10)) / 2;
			draw_string(display, window, gc, 0xFFFFFF, adMsgX, 120, adMsg);
		
		//internal registers
			char irMsg[50] = "IR1-[";
			char hashTemp5[5];
			draw_hex((uint32_t)cpu.IR1, 2, hashTemp5);
			strcat(irMsg, hashTemp5);
			
			strcat(irMsg, "] IR2-[");
			char hashTemp6[5];
			draw_hex((uint32_t)cpu.IR2, 2, hashTemp6);
			strcat(irMsg, hashTemp6);
			strcat(irMsg, "]");
			
			int irMsgX = 780 + (int)(200 - (strlen(irMsg)*10)) / 2;
			draw_string(display, window, gc, 0xFFFFFF, irMsgX, 150, irMsg);
		
		//addressable registers
			char arMsg[50] = "AR0-[";
			char hashTemp7[5];
			draw_hex((uint32_t)cpu.AR0, 2, hashTemp7);
			strcat(arMsg, hashTemp7);
			
			strcat(arMsg, "] AR1-[");
			char hashTemp8[5];
			draw_hex((uint32_t)cpu.AR1, 2, hashTemp8);
			strcat(arMsg, hashTemp8);
			strcat(arMsg, "]");
			
			int arMsgX = 780 + (int)(200 - (strlen(arMsg)*10)) / 2;
			draw_string(display, window, gc, 0xFFFFFF, arMsgX, 180, arMsg);
			
			char arMsg2[50] = "AR2-[";
			char hashTemp9[5];
			draw_hex((uint32_t)cpu.AR2, 2, hashTemp9);
			strcat(arMsg2, hashTemp9);
			
			strcat(arMsg2, "] AR3-[");
			char hashTemp11[5];
			draw_hex((uint32_t)cpu.AR3, 2, hashTemp11);
			strcat(arMsg2, hashTemp11);
			strcat(arMsg2, "]");
			int arMsg2X = 780 + (int)(200 - (strlen(arMsg2)*10)) / 2;
			draw_string(display, window, gc, 0xFFFFFF, arMsg2X, 210, arMsg2);
			
		//drawflags to highlight current things
			
			//PCO
				if(cpu_get_drawflag(PCO) == 1){
				
					char hpcMsg[50] = "PCO-[";
					char hHashTemp[5];
					draw_hex((uint32_t)cpu.PCO, 2, hHashTemp);
					strcat(hpcMsg, hHashTemp);
					strcat(hpcMsg, "]");
					draw_string(display, window, gc, 0xFF00E8, 795, 90, hpcMsg);
				
				}
				
			//OPC
				if(cpu_get_drawflag(OPC) == 1){
				
					char hpcMsg[50] = "OPC-[";
					char hHashTemp[5];
					draw_hex((uint32_t)cpu.OPC, 2, hHashTemp);
					strcat(hpcMsg, hHashTemp);
					strcat(hpcMsg, "]");
					draw_string(display, window, gc, 0xFF00E8, 885, 90, hpcMsg);
				
				}
				
			//STP
				if(cpu_get_drawflag(STP) == 1){
				
					char hpcMsg[50] = "STP-[";
					char hHashTemp[5];
					draw_hex((uint32_t)cpu.STP, 2, hHashTemp);
					strcat(hpcMsg, hHashTemp);
					strcat(hpcMsg, "]");
					draw_string(display, window, gc, 0xFF00E8, 795, 120, hpcMsg);
					
				}
				
			//CRE
				if(cpu.CRE > 0){
				
					char hpcMsg[50] = "CRE-[";
					char hHashTemp[5];
					draw_hex((uint32_t)cpu.CRE, 2, hHashTemp);
					strcat(hpcMsg, hHashTemp);
					strcat(hpcMsg, "]");
					draw_string(display, window, gc, 0xFF00E8, 885, 120, hpcMsg);
					
				}
				
			//IR1
				if(cpu_get_drawflag(IR1) == 1){
				
					char hpcMsg[50] = "IR1-[";
					char hHashTemp[5];
					draw_hex((uint32_t)cpu.IR1, 2, hHashTemp);
					strcat(hpcMsg, hHashTemp);
					strcat(hpcMsg, "]");
					draw_string(display, window, gc, 0xFF00E8, 795, 150, hpcMsg);
				
				}
				
			//IR2
				if(cpu_get_drawflag(IR2) == 1){
					char hpcMsg[50] = "IR2-[";
					char hHashTemp[5];
					draw_hex((uint32_t)cpu.IR2, 2, hHashTemp);
					strcat(hpcMsg, hHashTemp);
					strcat(hpcMsg, "]");
					draw_string(display, window, gc, 0xFF00E8, 885, 150, hpcMsg);
				}
				
			//AR0
				if(cpu_get_drawflag(AR0) == 1){
				
					char hpcMsg[50] = "AR0-[";
					char hHashTemp[5];
					draw_hex((uint32_t)cpu.AR0, 2, hHashTemp);
					strcat(hpcMsg, hHashTemp);
					strcat(hpcMsg, "]");
					draw_string(display, window, gc, 0xFF00E8, 795, 180, hpcMsg);
				
				}
				
			//AR1
				if(cpu_get_drawflag(AR1) == 1){
					char hpcMsg[50] = "AR1-[";
					char hHashTemp[5];
					draw_hex((uint32_t)cpu.AR1, 2, hHashTemp);
					strcat(hpcMsg, hHashTemp);
					strcat(hpcMsg, "]");
					draw_string(display, window, gc, 0xFF00E8, 885, 180, hpcMsg);
				}
				
			//AR2
				if(cpu_get_drawflag(AR2) == 1){
					char hpcMsg[50] = "AR2-[";
					char hHashTemp[5];
					draw_hex((uint32_t)cpu.AR2, 2, hHashTemp);
					strcat(hpcMsg, hHashTemp);
					strcat(hpcMsg, "]");
					draw_string(display, window, gc, 0xFF00E8, 795, 210, hpcMsg);
				}
				
			//AR3
				if(cpu_get_drawflag(AR3) == 1){
					char hpcMsg[50] = "AR3-[";
					char hHashTemp[5];
					draw_hex((uint32_t)cpu.AR3, 2, hHashTemp);
					strcat(hpcMsg, hHashTemp);
					strcat(hpcMsg, "]");
					draw_string(display, window, gc, 0xFF00E8, 885, 210, hpcMsg);
				}
				
			//cpu status flags
				if(cpu.C == true){
					draw_string(display, window, gc, 0x22d816, 805, 240, "C:1");
				} else {
					draw_string(display, window, gc, 0xd81616, 805, 240, "C:0");
				}
				
				if(cpu.Z == true){
					draw_string(display, window, gc, 0x22d816, 845, 240, "Z:1");
				} else {
					draw_string(display, window, gc, 0xd81616, 845, 240, "Z:0");
				}
				
				if(cpu.V == true){
					draw_string(display, window, gc, 0x22d816, 885, 240, "V:1");
				} else {
					draw_string(display, window, gc, 0xd81616, 885, 240, "V:0");
				}
				
				if(cpu.N == true){
					draw_string(display, window, gc, 0x22d816, 925, 240, "N:1");
				} else {
					draw_string(display, window, gc, 0xd81616, 925, 240, "N:0");
				}
	
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
		
		//draw mem table header in cyan
			char topLine[120] = " RAM";
			
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
			
			strcat(topLine, "  HEX -> ASCII");
			
			draw_string(display, window, gc, 0x28d9ed, 10, y, topLine);
			
		//now draw bulk
			y += lineHeight;
			
			for (int row = 0; row < 16; row++){
			
				//split memory into two blocks
					if(row == 8){
					
						y += 30;
						
						//draw mem table header in cyan
						char topLine2[120] = " ROM";
						
						for (int col = 0; col < 16; col++){
						
							if(col == 8){
								strcat(topLine2, "  ");
							}
							
							if(col == 0){
								strcat(topLine2, " 00");
							} else {
								strcat(topLine2, " ");
								char hashTemp2[5];
								draw_hex((uint32_t)col, 2, hashTemp2);
								strcat(topLine2, hashTemp2);
							}
							
						}
						
						strcat(topLine2, "  HEX -> ASCII");
						draw_string(display, window, gc, 0x28d9ed, 10, y, topLine2);
						
						y += 30;
						
					}
			
				//draw the starting address of this line of bytes
					char memStart[8] = " $";
					char hashTemp[5];
					draw_hex((uint32_t)nAddr, 2, hashTemp);
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
						
						if(nAddr == cpu.PCO-1 && cpu.PCO-1 >= PROG_MEM_LOC){
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
					draw_string(display, window, gc, 0xffffff, 50, y, line);
				
				y += 30;
				
			}
			
		//draw locations AFTER the above, so it's atop it
		//PC Hex value
			pcX = 50 + (pcCol*30) + ((pcCol >= 8) ? 20 : 0);
			
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
			pcX = 570 + (pcCol*10) + ((pcCol >= 8) ? 10 : 0);
			
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
		draw_string(display, window, gc, 0x28d9ed, msgX, 330, msg);
	
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
					draw_string(display, window, gc, 0x28d9ed, lineX, lineY, lines[i]);
				} else {
					//draw white
					draw_string(display, window, gc, 0xFFFFFF, lineX, lineY, lines[i]);
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
		draw_string(display, window, gc, 0xFFFFFF, msgX, 630, msg);
	
	}
	
/**
* Dissassemble the memory program into coded lines to display
* @return void
*/
	void code_disassemble(){
	
		for(uint8_t i = 0; i < 128; i++){
			char line [50] = "$";
			char hex[5];
			draw_hex(i, 2, hex);
			strcat(line, hex);
			strcat(line, ": NOP c2");
			strcpy(code[i], line);
		}
	
		uint16_t addr = PROG_MEM_LOC;
		uint16_t lineAddr;
		
		while(addr < MEM_LENGTH){
		
			lineAddr = addr;
			char line[100] = "$";
			char hashTemp[5]; //+1 for \0
			draw_hex(addr, 2, hashTemp);
			strcat(line, hashTemp);
			strcat(line, ": ");
			
			uint8_t opcode = cpu.RAM[addr];
			strcat(line, instructions[opcode].name);
			
			char cycles[10];
			sprintf(cycles, " c%d", instructions[opcode].cycles);
			strcat(line, cycles);
			
			addr++;
			
			
			if(
				instructions[opcode].operate == &cpu_ins_NOP
			){
			
				strcpy(code[addr], "---");
				
			} else if(
				instructions[opcode].operate == &cpu_ins_BMP ||
				instructions[opcode].operate == &cpu_ins_SQR ||
				instructions[opcode].operate == &cpu_ins_JMP ||
				instructions[opcode].operate == &cpu_ins_JNE ||
				instructions[opcode].operate == &cpu_ins_JZS ||
				instructions[opcode].operate == &cpu_ins_JSR
			){
			
				strcpy(code[addr], "---");
				addr++;
			
			} else if(
				instructions[opcode].operate == &cpu_ins_LRV ||
				instructions[opcode].operate == &cpu_ins_LRM ||
				instructions[opcode].operate == &cpu_ins_LRR ||
				instructions[opcode].operate == &cpu_ins_STV ||
				instructions[opcode].operate == &cpu_ins_STR ||
				instructions[opcode].operate == &cpu_ins_AND ||
				instructions[opcode].operate == &cpu_ins_BOR ||
				instructions[opcode].operate == &cpu_ins_NDR ||
				instructions[opcode].operate == &cpu_ins_ORR
			){
			
				strcpy(code[addr], "---");
				addr++;
				strcpy(code[addr], "---");
				addr++;
			
			}
			
			strcpy(code[lineAddr], line);
		
		}
		
		for(uint16_t i = PROG_MEM_LOC; i < MEM_LENGTH; i++){
			//printf("%s\n", code[i]);
		}
	
	}

/**
* See if char is valid hex char to convert
* @param c
* @return bool
*/
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

		//process file
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
									
										//RFS means we can close current subroutine count and set where to jump now
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
									else if (strcmp((char*)command, "BOR") == 0){ cpu.RAM[addr] = 0x1a; }
									else if (strcmp((char*)command, "NDR") == 0){ cpu.RAM[addr] = 0x1b; }
									else if (strcmp((char*)command, "ORR") == 0){ cpu.RAM[addr] = 0x1c; }
							
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
												cpu.RAM[addr] = (uint8_t)(j);
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
