#include "cpu.h"

//main CPU structure
	CPU cpu;
	
/**
* Instruction opcode lookup table
*/
	INSTRUCTION instructions[16] = {
		{NOP, "NOP", "No Operation", &cpu_ins_NOP, &cpu_am_AMN, },                  //0x0000
		{LDR, "LDR", "Load Register, Immediate", &cpu_ins_LRR, &cpu_am_AMI, },      //0x0001
		{LDR, "LDR", "Load Register, Memory", &cpu_ins_LRR, &cpu_am_AMM, },         //0x0002
		{STR, "STR", "Store Register, Immediate", &cpu_ins_STR, &cpu_am_AMI, },     //0x0003
		{NOP, "NOP", "No Operation", &cpu_ins_NOP, &cpu_am_AMN, },                  //0x0004
		{NOP, "NOP", "No Operation", &cpu_ins_NOP, &cpu_am_AMN, },                  //0x0005
		{NOP, "NOP", "No Operation", &cpu_ins_NOP, &cpu_am_AMN, },                  //0x0006
		{NOP, "NOP", "No Operation", &cpu_ins_NOP, &cpu_am_AMN, },                  //0x0007
		{NOP, "NOP", "No Operation", &cpu_ins_NOP, &cpu_am_AMN, },                  //0x0008
		{NOP, "NOP", "No Operation", &cpu_ins_NOP, &cpu_am_AMN, },                  //0x0009
		{NOP, "NOP", "No Operation", &cpu_ins_NOP, &cpu_am_AMN, },                  //0x000a
		{NOP, "NOP", "No Operation", &cpu_ins_NOP, &cpu_am_AMN, },                  //0x000b
		{NOP, "NOP", "No Operation", &cpu_ins_NOP, &cpu_am_AMN, },                  //0x000c
		{NOP, "NOP", "No Operation", &cpu_ins_NOP, &cpu_am_AMN, },                  //0x000d
		{NOP, "NOP", "No Operation", &cpu_ins_NOP, &cpu_am_AMN, },                  //0x000e
		{NOP, "NOP", "No Operation", &cpu_ins_NOP, &cpu_am_AMN, }                   //0x000f
	};
	
/*
* Read a byte from memory
* @return uint8_t
*/
	uint8_t cpu_read(uint8_t addr){
		cpu.PCO++;
		return cpu.RAM[addr];
	}
	
/*
* Write a byte to memory
* @return void
*/
	void cpu_write(uint8_t addr, uint8_t data){
		cpu.PCO++;
		cpu.RAM[addr] = data;
	}
	
////////////////////////////////////////////////////////////////////////////////////////
//ADDRESS MODES

	/*
	* CPU Address mode: None
	* @return uint8_t
	*/
		uint8_t cpu_am_AMN(){
			cpu.ADM = NoAddress;
			return 1;
		}
		
	/*
	* CPU Address mode: Immediate
	* Use the VALUE in the next byte
	* @return uint8_t
	*/
		uint8_t cpu_am_AMI(){
			cpu.ADM = Immediate;
			return 3;
		}
		
	/*
	* CPU Address mode: Memory
	* Use the value of the MEMORY ADDRESS in the next byte
	* @return uint8_t
	*/
		uint8_t cpu_am_AMM(){
			cpu.ADM = Memory;
			return 4;
		}
		
////////////////////////////////////////////////////////////////////////////////////////
//INSTRUCTIONS MODES

/*
* Instruction: No Operation
* @return void
*/
	void cpu_ins_NOP(){
		//execute NOP
			return;
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
						cpu.IR1 = cpu_read(cpu.PCO);
					break;
					
				//load IR2 with the value at mem location
					case 2:
						cpu.IR2 = cpu_read(cpu.PCO);
					break;
					
				//load the value at IR2 into register chosen in IR1
					case 1: {
					
						switch(cpu.IR1){
							
							case 0:
							default:
								cpu.AR0 = cpu.IR2;
							break;
							case 1:
								cpu.AR1 = cpu.IR2;
							break;
							case 2:
								cpu.AR2 = cpu.IR2;
							break;
							
						}
					
					} break;
			
			}
		
		} else {
		
			switch(cpu.CRE){
			
				//load IR1 with the identifier for which register to finally load
					case 4:
						cpu.IR1 = cpu_read(cpu.PCO);
					break;
					
				//load IR2 with the mem address for the value
					case 3:
						cpu.IR2 = cpu_read(cpu.PCO);
					break;
					
				//load the value stored at mem location in IR2 into IR2
					case 2:
						cpu.IR2 = cpu_read(cpu.IR2);
					break;
					
				//load the value at IR2 into register chosen in IR1
					case 1: {
					
						switch(cpu.IR1){
							
							case 0:
							default:
								cpu.AR0 = cpu.IR2;
							break;
							case 1:
								cpu.AR1 = cpu.IR2;
							break;
							case 2:
								cpu.AR2 = cpu.IR2;
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
					cpu.IR1 = cpu_read(cpu.PCO);
				break;
				
			//load IR2 with address of the destination memory location
				case 2:
					cpu.IR2 = cpu_read(cpu.PCO);
				break;
				
			//load the value at IR2 into register chosen in IR1
				case 1: {
				
					switch(cpu.IR1){
						
						case 0:
						default:
							cpu_write(cpu.AR0, cpu.IR2);
						break;
						case 1:
							cpu_write(cpu.AR1, cpu.IR2);
						break;
						case 2:
							cpu_write(cpu.AR2, cpu.IR2);
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
				
				int i = 1;
				while(1){
					
					//get next char
						c = (uint8_t)fgetc(fp);
			
					//check if end of file or end of memory now
						if(feof(fp) || bytes > 128){
							break;
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
		
		//reset program counter to start of program memory (ROM)
			cpu.PCO = 0x80;
				//@1
				
		//set remaing cycles to 0, which indicates read new instruction
			cpu.CRE = 0;
		
	}
	
	void cpu_execute(){
	
		if(cpu.CRE == 0){
		
			//read next instruction
				cpu.IR1 = cpu_read(cpu.PCO);
				//@1
				cpu.INS = instructions[cpu.IR1];
				
			//set OP
				cpu.OPC = (uint8_t)cpu.IR1;
				
			//set address mode by calling function
			//address mode will always add 0+ more cycles
				cpu.CRE = (*instructions[cpu.OPC].addrmode)();
		
		} else {
		
			(*instructions[cpu.OPC].operate)();
			//@1
			
			cpu.CRE--;
		
		}
	
	}
