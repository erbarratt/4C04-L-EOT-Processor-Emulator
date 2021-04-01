#include "cpu.h"

//main CPU structure
	DRAWFLAGS drawflags;
	CPU cpu;
	
/**
* Instruction opcode lookup table
*/
	INSTRUCTION instructions[64] = {
		{"NOP", "No Operation",                        &cpu_ins_NOP, 2},       //00
		
		{"LRV", "Load Register, from Value",           &cpu_ins_LRV, 3},       //01 [#reg0-2] [#val]
		{"LRM", "Load Register, from Memory",          &cpu_ins_LRM, 4},       //02 [#reg0-2] [$mem]
		{"LRR", "Load Register, from Register",        &cpu_ins_LRR, 4},       //03 [#reg0-2] [#reg0-2]
		
		{"NOP", "No Operation",                        &cpu_ins_NOP, 2},       //04
		{"NOP", "No Operation",                        &cpu_ins_NOP, 2},       //05
		
		{"STV", "Store Register, from Memory",         &cpu_ins_STV, 3},       //06 [#reg0-2] [$mem]
		{"STR", "Store Register, from Register",       &cpu_ins_STR, 4},       //07 [#reg0-2] [#reg0-2] Stores first reg in mem loc of second reg
		
		{"NOP", "No Operation",                        &cpu_ins_NOP, 2},       //08
		{"NOP", "No Operation",                        &cpu_ins_NOP, 2},       //09
		
		{"BMP", "Bump Register",                       &cpu_ins_BMP, 4},       //0a [#reg0-2]++
		{"SQR", "Squash Register",                     &cpu_ins_SQR, 4},       //0b [#reg0-2]--
		
		{"NOP", "No Operation",                        &cpu_ins_NOP, 2},       //0c
		{"NOP", "No Operation",                        &cpu_ins_NOP, 2},       //0d
		
		{"ADD", "Add RX to Ry, Store in RX",           &cpu_ins_ADD, 2},       //0e [#reg0-2] [#reg0-2] Stores in first reg defined
		{"SUB", "Sub RY from RX, Store in RX",         &cpu_ins_SUB, 2},       //0f [#reg0-2] [#reg0-2] Stores in first reg defined
		
		{"NOP", "No Operation",                        &cpu_ins_NOP, 2},       //10
		{"NOP", "No Operation",                        &cpu_ins_NOP, 2},       //11
		
		{"JMP", "Jump, from Value",                    &cpu_ins_JMP, 2},       //12 [$mem]
		{"JNE", "Jump if Zero not set",                &cpu_ins_JNE, 2},       //13 [$mem]
		{"JZS", "Jump if Zero set",                    &cpu_ins_JZS, 2},       //14 [$mem]
		{"JSR", "Jump to Subroutine",                  &cpu_ins_JSR, 3},       //15 [$mem]
		{"RFS", "Return from Subroutine",              &cpu_ins_RFS, 2},       //16
		
		{"NOP", "No Operation",                        &cpu_ins_NOP, 2},       //17
		{"NOP", "No Operation",                        &cpu_ins_NOP, 2},       //18
		
		{"AND", "Bitwise AND, from Value",             &cpu_ins_AND, 4},       //19 [#reg0-2] [#val]
		{"BOR", "Bitwise OR, from Value",              &cpu_ins_BOR, 4},       //1a [#reg0-2] [#val]
		{"NDR", "Bitwise AND, from Register",          &cpu_ins_NDR, 4},       //1b [#reg0-2] [#reg0-2] Stores in first reg defined
		{"ORR", "Bitwise OR, from Register",           &cpu_ins_ORR, 4},       //1c [#reg0-2] [#reg0-2] Stores in first reg defined
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
	
/*
* Write a byte to the top of the stack
* @param uint8_t data
* @return void
*/
	void cpu_stack_push(uint8_t data){
		cpu_write(cpu.STP, data);
		cpu.STP--;
		cpu_set_draw_flag(STP, true);
	}
	
/*
* Read a byte from the top of the stack
* @return uint8_t
*/
	uint8_t cpu_stack_pop(){
		cpu.STP++;
		uint8_t data = cpu.RAM[cpu.STP];
		cpu_set_draw_flag(STP, true);
		return data;
	}
	
/*
* Get the value in an addressable register
* @param uint8_t reg Which register
* @return uint8_t
*/
	uint8_t cpu_reg_val(uint8_t reg){
	
		switch(reg){
			
			case 0:
			default:
				return cpu.AR0;
			case 1:
				return cpu.AR1;
			case 2:
				return cpu.AR2;
			case 3:
				return cpu.AR3;
			
		}
	
	}
	
/*
* Set the value in an addressable register
* @param uint8_t reg Which register
* @param uint8_t data
* @return uint8_t
*/
	uint8_t cpu_set_reg(uint8_t reg, uint8_t data){
	
		switch(reg){
			
			case 0:
			default:
				cpu.AR0 = data;
				cpu_set_draw_flag(AR0, true);
			break;
			case 1:
				cpu.AR1= data;
				cpu_set_draw_flag(AR1, true);
			break;
			case 2:
				cpu.AR2= data;
				cpu_set_draw_flag(AR2, true);
			break;
			case 3:
				cpu.AR3= data;
				cpu_set_draw_flag(AR3, true);
			break;
			
		}
		
		return data;
	
	}
	
/*
* Set the value in an internal register
* @param uint8_t reg Which register
* @param uint8_t data
* @return uint8_t
*/
	uint8_t cpu_set_ir(uint8_t reg, uint8_t data){
	
		switch(reg){
		
			case 1:
			default:
				cpu.IR1= data;
				cpu_set_draw_flag(IR1, true);
			break;
			case 2:
				cpu.IR2= data;
				cpu_set_draw_flag(IR2, true);
			break;
			
		}
		
		return data;
	
	}
	
/*
* Set the PCO
* @param uint8_t data
* @return uint8_t
*/
	uint8_t cpu_set_pco(uint8_t data){
	
		cpu.PCO= data;
		cpu_set_draw_flag(PCO, true);
		return data;
	
	}
	
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//INSTRUCTIONS

/*
* Instruction: No Operation
* @return void
*/
	void cpu_ins_NOP(){
	}
	
/*
* Instruction: Load register from value
* 3 cycles
* Z,N
* @return void
*/
	void cpu_ins_LRV(){
	
		switch(cpu.CRE){
			
			//load IR1 with the identifier for which register to finally load
				case 3:
					cpu_set_ir(1, cpu_read(cpu.PCO, true));
				break;
				
			//load IR2 with the value at mem location
				case 2:
					cpu_set_ir(2, cpu_read(cpu.PCO, true));
				break;
				
			//load the value at IR2 into register chosen in IR1
				case 1:
					cpu_set_reg(cpu.IR1, cpu.IR2);
					cpu.Z = (cpu.IR2 == 0x00);
					cpu.N = ((cpu.IR2 & 0x0080) == 0x0080);
				break;
		
		}
		
	}
	
/*
* Instruction: Load register from memory addr
* 4 cycles
* Z,N
* @return void
*/
	void cpu_ins_LRM(){
	
		switch(cpu.CRE){
			
			//load IR1 with the identifier for which register to finally load
				case 4:
					cpu_set_ir(1, cpu_read(cpu.PCO, true));
				break;
				
			//load IR2 with the mem address for the value
				case 3:
					cpu_set_ir(2, cpu_read(cpu.PCO, true));
				break;
				
			//load the value stored at mem location in IR2 into IR2
				case 2:
					cpu_set_ir(2, cpu_read(cpu.IR2, false));
				break;
				
			//load the value at IR2 into register chosen in IR1, flags
				case 1:
					cpu_set_reg(cpu.IR1, cpu.IR2);
					cpu.Z = (cpu.IR2 == 0x00);
					cpu.N = ((cpu.IR2 & 0x0080) == 0x0080);
				break;
		
		}
		
	}
	
/*
* Instruction: Load register from Register
* 4 cycles
* Z,N
* @return void
*/
	void cpu_ins_LRR(){
	
		switch(cpu.CRE){
			
			//load IR1 with the identifier for which register to finally load
				case 4:
					cpu_set_ir(1, cpu_read(cpu.PCO, true));
				break;
				
			//Set IR2 to be the reigster to take the mem loc from
				case 3:
					cpu_set_ir(2, cpu_read(cpu.PCO, true));
				break;
				
			//set IR2 to be the value of the register set in IR2
				case 2:
					cpu_set_ir(2, cpu_read(cpu_reg_val(cpu.IR2), false));
				break;
				
			//load the value at IR2 into register chosen in IR1
				case 1:
					cpu_set_reg(cpu.IR1, cpu.IR2);
					cpu.Z = (cpu.IR2 == 0x00);
					cpu.N = ((cpu.IR2 & 0x0080) == 0x0080);
				break;
		
		}
		
	}
	
/*
* Instruction: Store register at memory adddress in next read value
* 3 cycles
* @return void
*/
	void cpu_ins_STV(){
	
		switch(cpu.CRE){
		
			//load IR1 with the identifier for which register to finally load
				case 3:
					cpu_set_ir(1, cpu_read(cpu.PCO, true));
				break;
				
			//load IR2 with address of the destination memory location
				case 2:
					cpu_set_ir(2, cpu_read(cpu.PCO, true));
				break;
				
			//store the value in the register chosen at IR1 into mem location IR2
				case 1:
					cpu_write(cpu.IR2, cpu_reg_val(cpu.IR1));
				break;
		
		}
	
	}
	
/*
* Instruction: Store register at memory adddress of register set in next byte
* x cycles
* @return void
*/
	void cpu_ins_STR(){
	
		switch(cpu.CRE){
		
			//load IR1 with the identifier for which register to finally load
				case 4:
					cpu_set_ir(1, cpu_read(cpu.PCO, true));
				break;
				
			//load IR2 with the id or the register where the mem location is
				case 3:
					cpu_set_ir(2, cpu_read(cpu.PCO, true));
				break;
				
			//get the memory location from the chosen register
				case 2:
					cpu_set_ir(2, cpu_reg_val(cpu.IR2));
				break;
				
			//store the value in the register chosen at IR1 into mem location IR2
				case 1:
					cpu_write(cpu.IR2, cpu_reg_val(cpu.IR1));
				break;
		
		}
	
	}
	
/*
* Instruction: Bump (Increment) value in register
* 4 cycles
* V,Z,N
* @return void
*/
	void cpu_ins_BMP(){
	
		switch(cpu.CRE){
		
			//load IR1 with the identifier for which register to bump
				case 4:
					cpu_set_ir(1, cpu_read(cpu.PCO, true));
				break;
				
			//set IR2 to be the value of the register set in IR2
				case 3:
					cpu_set_ir(2, cpu_reg_val(cpu.IR1));
				break;
				
			//do bump
				case 2:{
				
					uint16_t temp = cpu.IR2;
					temp++;
					cpu_set_ir(2, (uint8_t) temp & 0xFF);
					
					//overflowed?
						cpu.V = (temp > 255);
					//now zero?
						cpu.Z = (cpu.IR2 == 0);
					//negative?
						cpu.N = ((cpu.IR2 & 0x0080) == 0x0080);
					
				} break;
				
			//store the value in the register chosen at IR1 from IR2
				case 1:{
					
					cpu_set_reg(cpu.IR1, cpu.IR2);
					
				} break;
		
		}
	
	}
	
/*
* Instruction: Squash (Decrement) value in register
* 4 cycles
* V,Z,N
* @return void
*/
	void cpu_ins_SQR(){
	
		switch(cpu.CRE){
		
			//load IR1 with the identifier for which register to bump
				case 4:
					cpu_set_ir(1, cpu_read(cpu.PCO, true));
				break;
				
			//set IR2 to be the value of the register set in IR2
				case 3:
					cpu_set_ir(2, cpu_reg_val(cpu.IR1));
				break;
				
			//do bump
				case 2:{
				
					uint16_t temp = cpu.IR2;
					temp--;
					cpu_set_ir(2, (uint8_t) temp & 0xFF);
					
					//overflowed?
						cpu.V = (temp > 255);
					//now zero?
						cpu.Z = (cpu.IR2 == 0);
					//negative?
						cpu.N = ((cpu.IR2 & 0x0080) == 0x0080);
					
				} break;
				
			//store the value in the register chosen at IR1 from IR2
				case 1:{
					
					cpu_set_reg(cpu.IR1, cpu.IR2);
					
				} break;
		
		}
	
	}
	

/**
* Instruction: Add with Carry In
*
* Explanation:
* The purpose of this function is to add a value to the accumulator and a carry bit. If
* the result is > 255 there is an overflow setting the carry bit. Ths allows you to
* chain together ADC instructions to add numbers larger than 8-bits. This in itself is
* simple, however the 6502 supports the concepts of Negativity/Positivity and Signed Overflow.
*
* 10000100 = 128 + 4 = 132 in normal circumstances, we know this as unsigned and it allows
* us to represent numbers between 0 and 255 (given 8 bits). The 6502 can also interpret
* this word as something else if we assume those 8 bits represent the range -128 to +127,
* i.e. it has become signed.
*
* Since 132 > 127, it effectively wraps around, through -128, to -124. This wraparound is
* called overflow, and this is a useful to know as it indicates that the calculation has
* gone outside the permissable range, and therefore no longer makes numeric sense.
*
* Note the implementation of ADD is the same in binary, this is just about how the numbers
* are represented, so the word 10000100 can be both -124 and 132 depending upon the
* context the programming is using it in. We can prove this!
*
*  10000100 =  132  or  -124
* +00010001 = + 17      + 17
*  ========    ===       ===     See, both are valid additions, but our interpretation of
*  10010101 =  149  or  -107     the context changes the value, not the hardware!
*
* In principle under the -128 to 127 range:
* 10000000 = -128, 11111111 = -1, 00000000 = 0, 00000000 = +1, 01111111 = +127
* therefore negative numbers have the most significant set, positive numbers do not
*
* To assist us, the 6502 can set the overflow flag, if the result of the addition has
* wrapped around. V <- ~(A^M) & A^(A+M+C) :D lol, let's work out why!
*
* Let's suppose we have A = 30, M = 10 and C = 0
*          A = 30 = 00011110
*          M = 10 = 00001010+
*     RESULT = 40 = 00101000
*
* Here we have not gone out of range. The resulting significant bit has not changed.
* So let's make a truth table to understand when overflow has occurred. Here I take
* the MSB of each component, where R is RESULT.
*
* A  M  R | V | A^R | A^M |~(A^M) |
* 0  0  0 | 0 |  0  |  0  |   1   |
* 0  0  1 | 1 |  1  |  0  |   1   |
* 0  1  0 | 0 |  0  |  1  |   0   |
* 0  1  1 | 0 |  1  |  1  |   0   |  so V = ~(A^M) & (A^R)
* 1  0  0 | 0 |  1  |  1  |   0   |
* 1  0  1 | 0 |  0  |  1  |   0   |
* 1  1  0 | 1 |  1  |  0  |   1   |
* 1  1  1 | 0 |  0  |  0  |   1   |
*
* We can see how the above equation calculates V, based on A, M and R. V was chosen
* based on the following hypothesis:
*       Positive Number + Positive Number = Negative Result -> Overflow
*       Negative Number + Negative Number = Positive Result -> Overflow
*       Positive Number + Negative Number = Either Result -> Cannot Overflow
*       Positive Number + Positive Number = Positive Result -> OK! No Overflow
*       Negative Number + Negative Number = Negative Result -> OK! NO Overflow
* @return uint8_t
*/

/*
* Instruction: Add - <reg1> + <reg2>
* 5 cycles
* C,V,Z,N
* @return void
*/
	void cpu_ins_ADD(){
	
		switch(cpu.CRE){
		
			//load IR1 with the identifier for which register to add and store answer in
				case 5:
					cpu_set_ir(1, cpu_read(cpu.PCO, true));
				break;
				
			//load IR2 with the identifier for which other register to add
				case 4:
					cpu_set_ir(2, cpu_read(cpu.PCO, true));
				break;
				
			//Get value of second register and resotre in IR2
				case 3:
					cpu_set_ir(2, cpu_reg_val(cpu.IR2));
				break;
				
			//do addition into IR2 setting carry as necessary
				case 2:
				{
				
					uint8_t regVal = cpu_reg_val(cpu.IR1);
					uint16_t temp = (uint16_t)((uint16_t)regVal + (uint16_t)cpu.IR2 + (uint16_t)cpu.C);
					
					// The signed Overflow flag
						cpu.V = (~((uint16_t)regVal ^ (uint16_t)cpu.IR2) & ((uint16_t)regVal ^ (uint16_t)temp)) & 0x0080;
					
					// The carry flag out exists in the high byte bit 0
						cpu.C = temp > 255;
						
					// The Zero flag is set if the result is 0
						cpu.Z = ((temp & 0x00FF) == 0);
						
					// The negative flag is set to the most significant bit of the result
						cpu.N = ((temp & 0x0080) == 0x0080);
						
					cpu_set_ir(2, (uint8_t)(temp & 0x00FF));
					
				}
				break;
				
			//load the value at IR2 into register chosen in IR1
				case 1:
					cpu_set_reg(cpu.IR1, cpu.IR2);
				break;
		
		}
	
	}
	
/**
* Instruction: Subtraction with Borrow In
* Function:    A = A - M - (1 - C)
* Flags Out:   C, V, N, Z
*
* Explanation:
* Given the explanation for ADC above, we can reorganise our data
* to use the same computation for addition, for subtraction by multiplying
* the data by -1, i.e. make it negative
*
* A = A - M - (1 - C)  ->  A = A + -1 * (M - (1 - C))  ->  A = A + (-M + 1 + C)
*
* To make a signed positive number negative, we can invert the bits and add 1
* (OK, I lied, a little bit of 1 and 2s complement :P)
*
*  5 = 00000101
* -5 = 11111010 + 00000001 = 11111011 (or 251 in our 0 to 255 range)
*
* The range is actually unimportant, because if I take the value 15, and add 251
* to it, given we wrap around at 256, the result is 10, so it has effectively
* subtracted 5, which was the original intention. (15 + 251) % 256 = 10
*
* Note that the equation above used (1-C), but this got converted to + 1 + C.
* This means we already have the +1, so all we need to do is invert the bits
* of M, the data(!) therfore we can simply add, exactly the same way we did
* before.
* @return uint8_t
*/
	
/*
* Instruction: Subtract <reg1> - <reg2>, store in reg1
* 5 cycles
* C,V,Z,N
* @return void
*/
	void cpu_ins_SUB(){
	
		switch(cpu.CRE){
		
			//load IR1 with the identifier for which register to subtract and store answer in
				case 5:
					cpu_set_ir(1, cpu_read(cpu.PCO, true));
				break;
				
			//load IR2 with the identifier for which other register to subtract
				case 4:
					cpu_set_ir(2, cpu_read(cpu.PCO, true));
				break;
				
			//Get value of second register and resotre in IR2
				case 3:
					cpu_set_ir(2, cpu_reg_val(cpu.IR2));
				break;
				
			//do subtraction into IR2 setting carry as necessary
				case 2:
				{
					uint16_t tempVal = ((uint16_t)cpu.IR2 ^ 0x00FF);
					uint8_t regVal = cpu_reg_val(cpu.IR1);
					uint16_t temp = (uint16_t)((uint16_t)regVal + tempVal + (uint16_t)cpu.C);
					
					// The carry flag out exists in the high byte bit 0
						cpu.C = temp > 255;
						
					// The Zero flag is set if the result is 0
						cpu.Z = ((temp & 0x00FF) == 0);
						
					// The negative flag is set to the most significant bit of the result
						cpu.N = ((temp & 0x0080) == 0x0080);
						
					cpu_set_ir(2, (uint8_t)(temp & 0x00FF));
					
				}
				break;
				
			//load the value at IR2 into register chosen in IR1
				case 1:
					cpu_set_reg(cpu.IR1, cpu.IR2);
				break;
		
		}
	
	}
	
/*
* Instruction: Jump
* 2 cycles
* @return void
*/
	void cpu_ins_JMP(){
	
		switch(cpu.CRE){
		
			//load IR1 with memory address to jump to
				case 2:
					cpu_set_ir(1, cpu_read(cpu.PCO, true));
				break;
				
			//load IR2 with address of the destination memory location
				case 1:
					cpu_set_pco(cpu.IR1);
				break;
		
		}
	
	}
	
	
/*
* Instruction: Jump if Zero = 0
* 2 cycles
* @return void
*/
	void cpu_ins_JNE(){
	
		switch(cpu.CRE){
		
			//load IR1 with memory address to jump to
				case 2:
					cpu_set_ir(1, cpu_read(cpu.PCO, true));
				break;
				
			//Set PCO on condition
				case 1:{
				
					//only set PCO if zero flag not set
					if(cpu.Z == false){
						cpu_set_pco(cpu.IR1);
					}
					
				} break;
		
		}
	
	}
	
/*
* Instruction: Jump if Zero = 1
* 2 cycles
* @return void
*/
	void cpu_ins_JZS(){
	
		switch(cpu.CRE){
		
			//load IR1 with memory address to jump to
				case 2:
					cpu_set_ir(1, cpu_read(cpu.PCO, true));
				break;
				
			//Set PCO on condition
				case 1:{
				
					//only set PCO if zero flag not set
					if(cpu.Z == true){
						cpu_set_pco(cpu.IR1);
					}
					
				} break;
		
		}
	
	}
	
/*
* Instruction: Jump to Subroutine
* 3 cycles
* @return void
*/
	void cpu_ins_JSR(){
	
		switch(cpu.CRE){
		
			//put the next mem location onto stack
				case 3:
					cpu_stack_push((uint8_t)(cpu.PCO+1));
				break;
			
			//load IR1 with memory address to jump to
				case 2:
					cpu_set_ir(1, cpu_read(cpu.PCO, true));
				break;
				
			//set new PCO
				case 1:
					cpu_set_pco(cpu.IR1);
				break;
		
		}
	
	}
	
/*
* Instruction: Return from Subroutine
* 2 cycles
* @return void
*/
	void cpu_ins_RFS(){
	
		switch(cpu.CRE){
		
			//Get the last mem loc from stack
				case 2:
					cpu_set_ir(1, cpu_stack_pop());
				break;
				
			//set new PCO
				case 1:
					cpu_set_pco(cpu.IR1);
				break;
		
		}
	
	}
	
	
/*
* Instruction: Bitwise AND
* 4 cycles
* @return void
*/
	void cpu_ins_AND(){
	
		switch(cpu.CRE){
		
			//load IR1 with the identifier for which register to do bitwise AND on
				case 4:
					cpu_set_ir(1, cpu_read(cpu.PCO, true));
				break;
				
			//load IR2 with the second value of the bitwise AND
				case 3:
					cpu_set_ir(2, cpu_read(cpu.PCO, true));
				break;
				
			//do the bitwise and using val from reg defined in IR1
				case 2:
					cpu_set_ir(2, cpu.IR2 & cpu_reg_val(cpu.IR1));
				break;
			
			//store back to original register
				case 1:
					cpu_set_reg(cpu.IR1, cpu.IR2);
					//now zero?
						cpu.Z = (cpu.IR2 == 0);
					//negative?
						cpu.N = ((cpu.IR2 & 0x0080) == 0x0080);
				break;
		}
	
	}
	
/*
* Instruction: Bitwise OR
* 4 cycles
* @return void
*/
	void cpu_ins_BOR(){
	
		switch(cpu.CRE){
		
			//load IR1 with the identifier for which register to do bitwise AND on
				case 4:
					cpu_set_ir(1, cpu_read(cpu.PCO, true));
				break;
				
			//load IR2 with the second value of the bitwise AND
				case 3:
					cpu_set_ir(2, cpu_read(cpu.PCO, true));
				break;
				
			//do the bitwise and using val from reg defined in IR1
				case 2:
					cpu_set_ir(2, cpu.IR2 | cpu_reg_val(cpu.IR1));
				break;
			
			//store back to original register
				case 1:
					cpu_set_reg(cpu.IR1, cpu.IR2);
					//now zero?
						cpu.Z = (cpu.IR2 == 0);
					//negative?
						cpu.N = ((cpu.IR2 & 0x0080) == 0x0080);
				break;
		}
	
	}
	
/*
* Instruction: Bitwise AND between registers
* 4 cycles
* @return void
*/
	void cpu_ins_NDR(){
	
		switch(cpu.CRE){
		
			//load IR1 with the identifier for which register to do bitwise AND on
				case 4:
					cpu_set_ir(1, cpu_read(cpu.PCO, true));
				break;
				
			//load IR2 with the second value of the bitwise AND
				case 3:
					cpu_set_ir(2, cpu_read(cpu.PCO, true));
				break;
				
			//do the bitwise and using val from reg defined in IR1
				case 2:
					cpu_set_ir(2, cpu_reg_val(cpu.IR1) & cpu_reg_val(cpu.IR2));
				break;
			
			//store back to original register
				case 1:
					cpu_set_reg(cpu.IR1, cpu.IR2);
					//now zero?
						cpu.Z = (cpu.IR2 == 0);
					//negative?
						cpu.N = ((cpu.IR2 & 0x0080) == 0x0080);
				break;
		}
	
	}
	
/*
* Instruction: Bitwise OR between registers
* 4 cycles
* @return void
*/
	void cpu_ins_ORR(){
	
		switch(cpu.CRE){
		
			//load IR1 with the identifier for which register to do bitwise AND on
				case 4:
					cpu_set_ir(1, cpu_read(cpu.PCO, true));
				break;
				
			//load IR2 with the second value of the bitwise AND
				case 3:
					cpu_set_ir(2, cpu_read(cpu.PCO, true));
				break;
				
			//do the bitwise and using val from reg defined in IR1
				case 2:
					cpu_set_ir(2, cpu_reg_val(cpu.IR1) | cpu_reg_val(cpu.IR2));
				break;
			
			//store back to original register
				case 1:
					cpu_set_reg(cpu.IR1, cpu.IR2);
					//now zero?
						cpu.Z = (cpu.IR2 == 0);
					//negative?
						cpu.N = ((cpu.IR2 & 0x0080) == 0x0080);
				break;
		}
	
	}
	
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//CPU STATE FUNCTIONS

/**
* Reset System to mem and cpu start state
* @return void
*/
	void system_restart(){
	
		//fill ram/rom with NOPS
			for(int i = 0; i < MEM_LENGTH; i++){
				cpu.RAM[i] = 0x00;
			}
		
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
			cpu.AR3 = 0x00;
				//@1
				
		//flag bits
			cpu.C = 0;
			cpu.Z = 0;
			cpu.V = 0;
			cpu.N = 0;
		
		//reset program counter to start of program memory (ROM)
			cpu.PCO = PROG_MEM_LOC;
			cpu.lastOpAddr = PROG_MEM_LOC;
				//@1
				
			cpu.OPC = 0x00;
				
		//reset stack pointer to bottom of stack
			cpu.STP = STACK_MEM_LOC;
				
		//set remaing cycles to 0, which indicates read new instruction
			cpu.CRE = 0;
			
		//reset drawflags for draw functions
			drawflags = 0x00;
		
	}
	
/**
* Execute current instruction or look for next instruction
* @return void
*/
	void cpu_execute(){
	
		//clear any drawflags before execution to see what's happened this cycle
			drawflags = 0x00;
	
		if(cpu.CRE == 0){
		
			//check to see if program has ended, if so loop back
				if(cpu.PCO > 0xfb){
					cpu.PCO = 0x80;
				}
		
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
		
		} else {
		
			(*instructions[cpu.OPC].operate)();
			//@1
			
			cpu.CRE--;
		
		}
	
	}
