#include "draw.h"
#include "cpu.h"

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
		int msgX = 790 + (int)(200 - (strlen(msg)*10)) / 2;
		draw_string(display, window, gc, 0x28d9ed, msgX, 30, msg);
		
		//instruction name
			char arMsg3[100] = "Instruction: ";
			strcat(arMsg3, instructions[cpu.OPC].name);
			int arMsg3X = 790 + (int)(200 - (strlen(arMsg3)*10)) / 2;
			draw_string(display, window, gc, 0xFFFFFF, arMsg3X, 60, arMsg3);
		
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
			
			int pcMsgX = 790 + (int)(200 - (strlen(pcMsg)*10)) / 2; //805
			draw_string(display, window, gc, 0xFFFFFF, pcMsgX, 90, pcMsg);
		
		//Adress mode and Cycles Remaining
			char adMsg[50] = "ADM-[";
			char hashTemp3[5];
			draw_hex((uint32_t)cpu.ADM, 2, hashTemp3);
			strcat(adMsg, hashTemp3);
			
			strcat(adMsg, "] CRE-[");
			char hashTemp4[5];
			draw_hex((uint32_t)cpu.CRE, 2, hashTemp4);
			strcat(adMsg, hashTemp4);
			strcat(adMsg, "]");
			
			int adMsgX = 790 + (int)(200 - (strlen(adMsg)*10)) / 2;
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
			
			int irMsgX = 790 + (int)(200 - (strlen(irMsg)*10)) / 2;
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
			
			int arMsgX = 790 + (int)(200 - (strlen(arMsg)*10)) / 2;
			draw_string(display, window, gc, 0xFFFFFF, arMsgX, 180, arMsg);
			
			char arMsg2[50] = "AR2-[";
			char hashTemp9[5];
			draw_hex((uint32_t)cpu.AR2, 2, hashTemp9);
			strcat(arMsg2, hashTemp9);
			strcat(arMsg2, "]");
			int arMsg2X = 790 + (int)(200 - (strlen(arMsg2)*10)) / 2;
			draw_string(display, window, gc, 0xFFFFFF, arMsg2X, 210, arMsg2);
			
		//drawflags to highlight current things
			
			//PCO
				if(cpu_get_drawflag(PCO) == 1){
				
					char hpcMsg[50] = "PCO-[";
					char hHashTemp[5];
					draw_hex((uint32_t)cpu.PCO, 2, hHashTemp);
					strcat(hpcMsg, hHashTemp);
					strcat(hpcMsg, "]");
					draw_string(display, window, gc, 0xFF00E8, 805, 90, hpcMsg);
				
				}
				
			//OPC
				if(cpu_get_drawflag(OPC) == 1){
				
					char hpcMsg[50] = "OPC-[";
					char hHashTemp[5];
					draw_hex((uint32_t)cpu.OPC, 2, hHashTemp);
					strcat(hpcMsg, hHashTemp);
					strcat(hpcMsg, "]");
					draw_string(display, window, gc, 0xFF00E8, 895, 90, hpcMsg);
				
				}
				
			//ADM
				if(cpu_get_drawflag(ADM) == 1){
				
					char hpcMsg[50] = "ADM-[";
					char hHashTemp[5];
					draw_hex((uint32_t)cpu.ADM, 2, hHashTemp);
					strcat(hpcMsg, hHashTemp);
					strcat(hpcMsg, "]");
					draw_string(display, window, gc, 0xFF00E8, 805, 120, hpcMsg);
				
				}
				
			//CRE
				if(cpu.CRE > 0){
					char hpcMsg[50] = "CRE-[";
					char hHashTemp[5];
					draw_hex((uint32_t)cpu.CRE, 2, hHashTemp);
					strcat(hpcMsg, hHashTemp);
					strcat(hpcMsg, "]");
					draw_string(display, window, gc, 0xFF00E8, 895, 120, hpcMsg);
				}
				
			//IR1
				if(cpu_get_drawflag(IR1) == 1){
				
					char hpcMsg[50] = "IR1-[";
					char hHashTemp[5];
					draw_hex((uint32_t)cpu.IR1, 2, hHashTemp);
					strcat(hpcMsg, hHashTemp);
					strcat(hpcMsg, "]");
					draw_string(display, window, gc, 0xFF00E8, 805, 150, hpcMsg);
				
				}
				
			//IR2
				if(cpu_get_drawflag(IR2) == 1){
					char hpcMsg[50] = "IR2-[";
					char hHashTemp[5];
					draw_hex((uint32_t)cpu.IR2, 2, hHashTemp);
					strcat(hpcMsg, hHashTemp);
					strcat(hpcMsg, "]");
					draw_string(display, window, gc, 0xFF00E8, 895, 150, hpcMsg);
				}
				
			//AR0
				if(cpu_get_drawflag(AR0) == 1){
				
					char hpcMsg[50] = "AR0-[";
					char hHashTemp[5];
					draw_hex((uint32_t)cpu.AR0, 2, hHashTemp);
					strcat(hpcMsg, hHashTemp);
					strcat(hpcMsg, "]");
					draw_string(display, window, gc, 0xFF00E8, 805, 180, hpcMsg);
				
				}
				
			//AR1
				if(cpu_get_drawflag(AR1) == 1){
					char hpcMsg[50] = "AR1-[";
					char hHashTemp[5];
					draw_hex((uint32_t)cpu.AR1, 2, hHashTemp);
					strcat(hpcMsg, hHashTemp);
					strcat(hpcMsg, "]");
					draw_string(display, window, gc, 0xFF00E8, 895, 180, hpcMsg);
				}
				
			//AR2
				if(cpu_get_drawflag(AR2) == 1){
					char hpcMsg[50] = "AR2-[";
					char hHashTemp[5];
					draw_hex((uint32_t)cpu.AR2, 2, hHashTemp);
					strcat(hpcMsg, hHashTemp);
					strcat(hpcMsg, "]");
					draw_string(display, window, gc, 0xFF00E8, 850, 210, hpcMsg);
				}
				
			//cpu status flags
				if(cpu.C == true){
					draw_string(display, window, gc, 0x22d816, 815, 240, "C:1");
				} else {
					draw_string(display, window, gc, 0xd81616, 815, 240, "C:0");
				}
				
				if(cpu.Z == true){
					draw_string(display, window, gc, 0x22d816, 855, 240, "Z:1");
				} else {
					draw_string(display, window, gc, 0xd81616, 855, 240, "Z:0");
				}
				
				if(cpu.V == true){
					draw_string(display, window, gc, 0x22d816, 895, 240, "V:1");
				} else {
					draw_string(display, window, gc, 0xd81616, 895, 240, "V:0");
				}
				
				if(cpu.N == true){
					draw_string(display, window, gc, 0x22d816, 935, 240, "N:1");
				} else {
					draw_string(display, window, gc, 0xd81616, 935, 240, "N:0");
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
		int pcCol = 0, pcY = 0, pcX = 0;
		uint8_t pcVal = 0;
		
		//draw mem table header in cyan
			char topLine[120] = "    ";
			
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
						
						//draw mem table header in cyan
						char topLine2[120] = "    ";
						
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
		int msgX = 790 + (int)(200 - (strlen(msg)*10)) / 2;
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
			
				int lineX = 790 + (int)(200 - (strlen(lines[i])*10)) / 2;
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
		
		char * msg = "Press Space to step through CPU Cycles. Press Q to quit or R to reset system.";
		int msgX = (int)(1000 - (strlen(msg)*10)) / 2;
		draw_string(display, window, gc, 0xFFFFFF, msgX, 630, msg);
	
	}
	
/**
* Dissassemble the memory program into coded lines to display
* @return void
*/
	void code_disassemble(){
	
		for(uint8_t i = 0; i < 128; i++){
			strcpy(code[i], "NOP {AMN}");
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
			addr++;
			
			if(instructions[opcode].addrmode == &cpu_am_AMN){
				strcpy(code[addr], "---");
				strcat(line, " {AMN}");
			} else if(instructions[opcode].addrmode == &cpu_am_AMI){
				strcpy(code[addr], "---");
				strcat(line, " {AMI}");
				addr++;
				strcpy(code[addr], "---");
				addr++;
			
			} else if(instructions[opcode].addrmode == &cpu_am_AMM){
				strcpy(code[addr], "---");
				strcat(line, " {AMM}");
				addr++;
				strcpy(code[addr], "---");
				addr++;
			
			}
			
			strcpy(code[lineAddr], line);
		
		}
	
	}
