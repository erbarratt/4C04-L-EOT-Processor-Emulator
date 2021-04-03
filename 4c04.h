#ifndef FOUR_C_OH_FOUR_H

	#define FOUR_C_OH_FOUR_H

	#include <stdint.h>         //uintx_t's
	#include <stdbool.h>
	#include <X11/Xlib.h>       //X11
	
	#define MEM_LENGTH 256
	#define PROG_MEM_LOC 0x80
	#define STACK_MEM_LOC 0x7F
	
	////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////
	//CPU
	
		/**
		* bit field for cpu status
		*/
			typedef enum
			{
			
				PCO = (1 << 0),	// Program Counter
				IR1 = (1 << 1),	// Internal Reg 1
				IR2 = (1 << 2),	// Internal Reg 2
				AR0 = (1 << 3),	// Addressable Reg 0
				AR1 = (1 << 4),	// Addressable Reg 1
				AR2 = (1 << 5),	// Addressable Reg 2
				AR3 = (1 << 6),	// Addressable Reg 3
				OPC = (1 << 7),	// Current Opcode
				STP = (1 << 8),	// Stack Pointer
				
			} DRAWFLAGS;
		
		/**
		* Instruction struct
		* @var struct INSTRUCTION
		* @param char[] name
		* @param char[] label
		* @param void (*operate)
		* @param uint8_t cycles
		*/
			typedef struct{
				char name[10];
				char label[50];
				void (*operate )(void);
				uint8_t cycles;
				uint8_t pcoShifts;
			} INSTRUCTION;
		
		/**
		* Instruction lookup array
		*/
			extern INSTRUCTION instructions[64];
		
		/**
		* Instruction struct
		* @var struct INSTRUCTION
		* @param uint8_t PCO
		* @param uint8_t STP
		* @param uint8_t OPC
		* @param uint8_t CRE
		* @param uint8_t IR1
		* @param uint8_t IR2
		* @param uint8_t AR0
		* @param uint8_t AR1
		* @param uint8_t AR2
		* @param uint8_t AR3
		* @param uint8_t RAM[MEM_LENGTH]
		* @param bool c
		* @param bool v
		* @param bool z
		* @param bool n
		* @param INSTRUCTION INS
		* @param uint8_t lastOpAddr
		*/
			typedef struct{
			
				//program counter
					uint8_t PCO;
					
				//stack pointer
					uint8_t STP;
				
				//current opcode
					uint8_t OPC;
					
				//remaining cycles
					uint8_t CRE;
			
				//internal registers, 8bits
					uint8_t IR1;
					uint8_t IR2;
				
				//addressable registers, 8 bits
					uint8_t AR0;
					uint8_t AR1;
					uint8_t AR2;
					uint8_t AR3;
				
				//ROM + RAM
					uint8_t RAM[MEM_LENGTH];
				
				//Carry flag
					bool C;
				
				//Zero Flag
					bool Z;
				
				//Overflow
					bool V;
					
				//Negative Flag
					bool N;
				
				//current instruction
					INSTRUCTION INS;
					
				//last op address
					uint8_t lastOpAddr;
				
			} CPU;
		
			extern CPU cpu;
		
		//cpu base functions
			void cpu_set_draw_flag(DRAWFLAGS f, bool v);
			uint8_t cpu_get_drawflag(DRAWFLAGS f);
			uint8_t cpu_read(uint8_t addr, bool incPCO);
			void cpu_write(uint8_t addr, uint8_t data);
			void cpu_stack_push(uint8_t data);
			uint8_t cpu_stack_pop();
			uint8_t cpu_reg_val(uint8_t reg);
			uint8_t cpu_set_reg(uint8_t reg, uint8_t data);
			uint8_t cpu_set_ir(uint8_t reg, uint8_t data);
			uint8_t cpu_set_pco(uint8_t data);
			
		//Instructions
			void cpu_ins_NOP();
			
			void cpu_ins_LRV();
			void cpu_ins_LRM();
			void cpu_ins_LRR();
			void cpu_ins_LRT();
			
			void cpu_ins_STV();
			void cpu_ins_STR();
			
			void cpu_ins_BMP();
			void cpu_ins_SQR();
			void cpu_ins_ADD();
			void cpu_ins_SUB();
			
			void cpu_ins_JMP();
			void cpu_ins_JOC();
			void cpu_ins_JSR();
			void cpu_ins_RFS();
			
			void cpu_ins_AND();
			void cpu_ins_BOR();
			void cpu_ins_NDR();
			void cpu_ins_ORR();
			void cpu_ins_XOV();
			void cpu_ins_XOR();
			
			void system_restart();
			void cpu_execute();
	
	////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////
	//DRAW

		#define COL_WHITE 0xffffff
		#define COL_ORANGE 0xff8146
		#define COL_CYAN 0x28d9ed
		#define COL_MAGENTA 0xFF00E8
		#define COL_GREEN 0x30cc00
		#define COL_RED 0xd81616
	
		void console_print(char * msg, int error);
		char * hex(uint32_t hex, size_t size, char * dest);
		uint8_t find_hex_from_command(uint8_t * command);
		void draw_string(Display * display, Window window, GC gc, unsigned long color, int x, int y, char * str);
		void draw_cpu(Display * display, Window window, GC gc);
		void draw_ram(Display * display, Window window, GC gc);
		void draw_code(Display * display, Window window, GC gc, uint8_t addr);
		void draw_all(Display * display, Window window, GC gc);
		void code_disassemble();
		bool program_is_hex_char(uint8_t c);
		uint8_t program_get_hex_char(uint8_t c);
		void program_load();

#endif

