#include "cpu.h"

//main CPU structure
	DRAWFLAGS drawflags;
	CPU cpu;
	
/**
* Instruction opcode lookup table
*/
	INSTRUCTION instructions[32] = {
		{NOP, "NOP", "No Operation", &cpu_ins_NOP, &cpu_am_AMN, 0},                  //00
		{LDR, "LDR", "Load Register, Immediate", &cpu_ins_LRR, &cpu_am_AMI, 0},      //01 [#reg0-2] [#val]
		{LDR, "LDR", "Load Register, Memory", &cpu_ins_LRR, &cpu_am_AMM, 0},         //02 [#reg0-2] [$mem]
		{LDR, "LDR", "Load Register, At Register", &cpu_ins_LRR, &cpu_am_AMR, 0},    //03 [#reg0-2] [#reg0-2]
		
		{NOP, "NOP", "No Operation", &cpu_ins_NOP, &cpu_am_AMN, 0},                  //04
		{NOP, "NOP", "No Operation", &cpu_ins_NOP, &cpu_am_AMN, 0},                  //05
		
		{STR, "STR", "Store Register, Immediate", &cpu_ins_STR, &cpu_am_AMI, 0},     //06 [#reg0-2] [$mem]
		{NOP, "STR", "Store Register, Memory", &cpu_ins_NOP, &cpu_am_AMM, 0},        //07 [#reg0-2] [#reg0-2] Stores first reg in mem loc of second reg
		
		{NOP, "NOP", "No Operation", &cpu_ins_NOP, &cpu_am_AMN, 0},                  //08
		{NOP, "NOP", "No Operation", &cpu_ins_NOP, &cpu_am_AMN, 0},                  //09
		
		{NOP, "BMP", "Bump Register, Immediate", &cpu_ins_BMP, &cpu_am_AMI, 1},      //0a [#reg0-2]++
		{NOP, "SQW", "Squash Register, Immediate", &cpu_ins_NOP, &cpu_am_AMN, 0},    //0b [#reg0-2]--
		
		{NOP, "NOP", "No Operation", &cpu_ins_NOP, &cpu_am_AMN, 0},                  //0c
		{NOP, "NOP", "No Operation", &cpu_ins_NOP, &cpu_am_AMN, 0},                  //0d
		
		{ADD, "ADD", "Add, Immediate", &cpu_ins_ADD, &cpu_am_AMI, 2},                //0e [#reg0-2] [#reg0-2] Stores in first reg defined
		{SUB, "SUB", "Subtract, Immediate", &cpu_ins_NOP, &cpu_am_AMI, 2},           //0f [#reg0-2] [#reg0-2] Stores in first reg defined
		{NOP, "JMP", "Jump, Immediate", &cpu_ins_NOP, &cpu_am_AMN, 0},               //10 [$mem]
	};
	
/*
* Set a flag for the draw functions to read
* @param DRAWFLAGS f The bit
* @param bool v On or off
* @return void
*/
	void cpu_set_draw_flag(DRAWFLAGS f, bool v){
		if(v)
			drawflags |= f;
		else
			drawflags &= ~f;
	}
	
/**
* Returns the value of a specific bit of the drawflags
* @return uint8_t
 */
	uint8_t cpu_get_drawflag(DRAWFLAGS f){
		return ((drawflags & f) > 0) ? 1 : 0;
	}
	
/*
* Read a byte from memory
* @param uint8_t addr
* @return uint8_t
*/
	uint8_t cpu_read(uint8_t addr, bool incPCO){
		if(incPCO){
			cpu.PCO++;
			cpu_set_draw_flag(PCO, true);
		}
		return cpu.RAM[addr];
	}
	
/*
* Write a byte to memory
* @param uint8_t addr
* @param uint8_t data
* @return void
*/
	void cpu_write(uint8_t addr, uint8_t data){
		cpu.RAM[addr] = data;
	}
	
////////////////////////////////////////////////////////////////////////////////////////
//ADDRESS MODES

	/*
	* CPU Address mode: None
	* @return uint8_t
	*/
		uint8_t cpu_am_AMN(){ cpu.ADM = NoAddress; return 1; }
		
	/*
	* CPU Address mode: Immediate
	* Use the VALUE in the next byte
	* @return uint8_t
	*/
		uint8_t cpu_am_AMI(){ cpu.ADM = Immediate; return 3; }
		
	/*
	* CPU Address mode: Memory
	* Use the value of the MEMORY ADDRESS in the next byte
	* @return uint8_t
	*/
		uint8_t cpu_am_AMM(){ cpu.ADM = Memory; return 4; }
		
	/*
	* CPU Address mode: At Register
	* Use the value of the Register in the next byte as the mem loc
	* @return uint8_t
	*/
		uint8_t cpu_am_AMR(){ cpu.ADM = AtRegister; return 4; }
		
////////////////////////////////////////////////////////////////////////////////////////
//INSTRUCTIONS

/*
* Instruction: No Operation
* @return void
*/
	void cpu_ins_NOP(){
	}
	
/*
* Instruction: Load register
* NoAddress: n/a
* Immediate: 3 cycles
* Memory: 4 cycles
* @return void
*/
	void cpu_ins_LRR(){
	
		if(cpu.ADM == Immediate){
		
			switch(cpu.CRE){
			
				//load IR1 with the identifier for which register to finally load
					case 3:
						cpu.IR1 = cpu_read(cpu.PCO, true);
						cpu_set_draw_flag(IR1, true);
					break;
					
				//load IR2 with the value at mem location
					case 2:
						cpu.IR2 = cpu_read(cpu.PCO, true);
						cpu_set_draw_flag(IR2, true);
					break;
					
				//load the value at IR2 into register chosen in IR1
					case 1: {
					
						switch(cpu.IR1){
							
							case 0:
							default:
								cpu.AR0 = cpu.IR2;
								cpu_set_draw_flag(AR0, true);
							break;
							case 1:
								cpu.AR1 = cpu.IR2;
								cpu_set_draw_flag(AR1, true);
							break;
							case 2:
								cpu.AR2 = cpu.IR2;
								cpu_set_draw_flag(AR2, true);
							break;
							
						}
					
					} break;
			
			}
		
		} else if (cpu.ADM == Memory) {
		
			switch(cpu.CRE){
			
				//load IR1 with the identifier for which register to finally load
					case 4:
						cpu.IR1 = cpu_read(cpu.PCO, true);
						cpu_set_draw_flag(IR1, true);
					break;
					
				//load IR2 with the mem address for the value
					case 3:
						cpu.IR2 = cpu_read(cpu.PCO, true);
						cpu_set_draw_flag(IR2, true);
					break;
					
				//load the value stored at mem location in IR2 into IR2
					case 2:
						cpu.IR2 = cpu_read(cpu.IR2, false);
						cpu_set_draw_flag(IR2, true);
					break;
					
				//load the value at IR2 into register chosen in IR1
					case 1: {
					
						switch(cpu.IR1){
							
							case 0:
							default:
								cpu.AR0 = cpu.IR2;
								cpu_set_draw_flag(AR0, true);
							break;
							case 1:
								cpu.AR1 = cpu.IR2;
								cpu_set_draw_flag(AR1, true);
							break;
							case 2:
								cpu.AR2 = cpu.IR2;
								cpu_set_draw_flag(AR2, true);
							break;
							
						}
					
					} break;
			
			}
		
		} else {
		
			switch(cpu.CRE){
			
				//load IR1 with the identifier for which register to finally load
					case 4:
						cpu.IR1 = cpu_read(cpu.PCO, true);
						cpu_set_draw_flag(IR1, true);
					break;
					
				//Set IR2 to be the reigster to take the mem loc from
					case 3: {
					
						cpu.IR2 = cpu_read(cpu.PCO, true);
						cpu_set_draw_flag(IR2, true);
					
					} break;
					
				//set IR2 to be the value of the register set in IR2
					case 2: {
					
						switch(cpu.IR2){
							
							case 0:
							default:
								cpu.IR2 = cpu.AR0;
							break;
							case 1:
								cpu.IR2 = cpu.AR1;
							break;
							case 2:
								cpu.IR2 = cpu.AR2;
							break;
							
						}
						
						cpu_set_draw_flag(IR2, true);
					
					}
					break;
					
				//load the value at IR2 into register chosen in IR1
					case 1: {
					
						switch(cpu.IR1){
							
							case 0:
							default:
								cpu.AR0 = cpu_read(cpu.IR2, false);
								cpu_set_draw_flag(AR0, true);
							break;
							case 1:
								cpu.AR1 = cpu_read(cpu.IR2, false);
								cpu_set_draw_flag(AR1, true);
							break;
							case 2:
								cpu.AR2 = cpu_read(cpu.IR2, false);
								cpu_set_draw_flag(AR2, true);
							break;
							
						}
					
					} break;
			
			}
		
		}
		
	}
	
/*
* Instruction: Load register
* NoAddress: n/a
* Immediate: 3 cycles
* Memory: n/a
* @return void
*/
	void cpu_ins_STR(){
	
		switch(cpu.CRE){
		
			//load IR1 with the identifier for which register to finally load
				case 3:
					cpu.IR1 = cpu_read(cpu.PCO, true);
					cpu_set_draw_flag(IR1, true);
				break;
				
			//load IR2 with address of the destination memory location
				case 2:
					cpu.IR2 = cpu_read(cpu.PCO, true);
					cpu_set_draw_flag(IR2, true);
				break;
				
			//store the value in the register chosen at IR1 into mem location IR2
				case 1:{
					switch(cpu.IR1){
						case 0:
						default:
							cpu_write(cpu.IR2, cpu.AR0);
						break;
						case 1:
							cpu_write(cpu.IR2, cpu.AR1);
						break;
						case 2:
							cpu_write(cpu.IR2, cpu.AR2);
						break;
					}
				} break;
		
		}
	
	}
	
/*
* Instruction: Bump (Increment) value in register
* NoAddress: n/a
* Immediate: 3 cycles
* Memory: n/a
* @return void
*/
	void cpu_ins_BMP(){
	
		switch(cpu.CRE){
		
			//load IR1 with the identifier for which register to bump
				case 4:
					cpu.IR1 = cpu_read(cpu.PCO, true);
					cpu_set_draw_flag(IR1, true);
				break;
				
			//set IR2 to be the value of the register set in IR2
				case 3: {
				
					switch(cpu.IR1){
						
						case 0:
						default:
							cpu.IR2 = cpu.AR0;
						break;
						case 1:
							cpu.IR2 = cpu.AR1;
						break;
						case 2:
							cpu.IR2 = cpu.AR2;
						break;
						
					}
					
					cpu_set_draw_flag(IR2, true);
				
				}
				break;
				
			//do bump
				case 2:
					cpu.IR2++;
				break;
				
			//store the value in the register chosen at IR1 from IR2
				case 1:{
					
					uint8_t temp = cpu.IR2;
				
					switch(cpu.IR1){
						case 0:
						default:
							cpu.AR0 = cpu.IR2;
							cpu_set_draw_flag(AR0, true);
						break;
						case 1:
							cpu.AR1 = cpu.IR2;
							cpu_set_draw_flag(AR1, true);
						break;
						case 2:
							cpu.AR2 = cpu.IR2;
							cpu_set_draw_flag(AR2, true);
						break;
					}
					
					//set flags
						//overflowed?
						cpu.V = (temp == 0);
						//now zero?
						cpu.Z = (temp == 0);
						//negative?
						cpu.N = (temp & 0x80);
					
				} break;
		
		}
	
	}
	
/*
* Instruction: Add - <reg1> + <reg2>
* NoAddress: n/a
* Immediate: 5 cycles
* Memory: n/a
* @return void
*/
	void cpu_ins_ADD(){
	
		switch(cpu.CRE){
		
			//load IR1 with the identifier for which register to add and store answer in
				case 5:
					cpu.IR1 = cpu_read(cpu.PCO, true);
					cpu_set_draw_flag(IR1, true);
				break;
				
			//load IR2 with the identifier for which other register to add
				case 4:
					cpu.IR2 = cpu_read(cpu.PCO, true);
					cpu_set_draw_flag(IR2, true);
				break;
				
			//Get value of second register and resotre in IR2
				case 3:
				{
					
					switch(cpu.IR2){
						
						case 0:
							cpu.IR2 = cpu.AR0;
						break;
						case 1:
							cpu.IR2 = cpu.AR1;
						break;
						case 2:
							cpu.IR2 = cpu.AR2;
						break;
						
					}
					
					cpu_set_draw_flag(IR2, true);
					
				}
				break;
				
			//do addition into IR2 setting carry as necessary
				case 2:
				{
				
					uint16_t temp;
				
					switch(cpu.IR1){
						
						case 0:
						default:
							temp = (uint16_t)cpu.IR2 + (uint16_t)cpu.AR0 + (uint16_t)cpu.C;
							// The signed Overflow flag
							cpu.V = (~((uint16_t)cpu.IR2 ^ (uint16_t)cpu.AR0) & ((uint16_t)cpu.IR2 ^ (uint16_t)temp)) & 0x0080;
						break;
						case 1:
							temp = (uint16_t)cpu.IR2 + (uint16_t)cpu.AR1 + (uint16_t)cpu.C;
							// The signed Overflow flag
							cpu.V = (~((uint16_t)cpu.IR2 ^ (uint16_t)cpu.AR1) & ((uint16_t)cpu.IR2 ^ (uint16_t)temp)) & 0x0080;
						break;
						case 2:
							temp = (uint16_t)cpu.IR2 + (uint16_t)cpu.AR2 + (uint16_t)cpu.C;
							// The signed Overflow flag
							cpu.V = (~((uint16_t)cpu.IR2 ^ (uint16_t)cpu.AR2) & ((uint16_t)cpu.IR2 ^ (uint16_t)temp)) & 0x0080;
						break;
						
					}
					
					// The carry flag out exists in the high byte bit 0
						cpu.C = temp > 255;
						
					// The Zero flag is set if the result is 0
						cpu.Z = ((temp & 0x00FF) == 0);
						
					// The negative flag is set to the most significant bit of the result
						cpu.N = temp & 0x80;
					
					cpu.IR2 = temp & 0x00FF;
					cpu_set_draw_flag(IR2, true);
					
				}
				break;
				
			//load the value at IR2 into register chosen in IR1
				case 1: {
				
					switch(cpu.IR1){
						
						case 0:
						default:
							cpu.AR0 = cpu.IR2;
							cpu_set_draw_flag(AR0, true);
						break;
						case 1:
							cpu.AR1 = cpu.IR2;
							cpu_set_draw_flag(AR1, true);
						break;
						case 2:
							cpu.AR2 = cpu.IR2;
							cpu_set_draw_flag(AR2, true);
						break;
						
					}
				
				} break;
		
		}
	
	}

/**
* Reset System to mem and cpu start state
* @return void
*/
	void system_restart(){
	
		//fill ram/rom with NOPS
			for(int i = 0; i < MEM_LENGTH; i++){
				cpu.RAM[i] = 0x00;
			}
			
		//load program into program memory
			FILE * fp;
			
			fp = fopen("prog.txt", "r");
			
			if (fp == NULL){
				printf("No Program File\n");
				exit(1);
			}
			
			//holding vars
				uint8_t c, new, tmp;
				uint8_t hi = 0x00;
				uint8_t lo;
				
				int bytes = 0;
				uint8_t nOffset = PROG_MEM_LOC;
				
				bool comment = false;
				
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
						
						if(comment){
							if(c == '\n'){
								comment = false;
							}
							continue;
						}
			
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
							cpu.RAM[nOffset] = new;
							nOffset++;
							bytes++;
						}
			
				}
				
				fclose(fp);
		
		//internal registers
			cpu.IR1 = 0x0000;
				//@1
			cpu.IR2 = 0x0000;
				//@1
			cpu.AR0 = 0x00;
				//@1
			cpu.AR1 = 0x00;
				//@1
			cpu.AR2 = 0x00;
				//@1
				
		//flag bits
			cpu.C = 0;
			cpu.Z = 0;
			cpu.V = 0;
			cpu.N = 0;
		
		//reset program counter to start of program memory (ROM)
			cpu.PCO = 0x80;
			cpu.lastOpAddr = 0x80;
				//@1
				
		//set remaing cycles to 0, which indicates read new instruction
			cpu.CRE = 0;
			
		//reset drawflags for draw functions
			drawflags = 0x00;
		
	}
	
	void cpu_execute(){
	
		//clear any drawflags before execution to see what's happened this cycle
			drawflags = 0x00;
	
		if(cpu.CRE == 0){
		
			//set last opcode location for drawing code lines
				cpu.lastOpAddr = cpu.PCO;
		
			//read next instruction
				cpu.IR1 = cpu_read(cpu.PCO, true);
				cpu_set_draw_flag(IR1, true);
				//@1
				cpu.INS = instructions[cpu.IR1];
				
			//set OP
				cpu.OPC = (uint8_t)cpu.IR1;
				cpu_set_draw_flag(OPC, true);
				
			//set default cycles
				cpu.CRE = cpu.INS.cycles;
				
			//set address mode by calling function
			//address mode will always add 0+ more cycles
				cpu.CRE += (*instructions[cpu.OPC].addrmode)();
				cpu_set_draw_flag(ADM, true);
		
		} else {
		
			(*instructions[cpu.OPC].operate)();
			//@1
			
			cpu.CRE--;
		
		}
	
	}
