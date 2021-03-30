#include <stdio.h>
#include <stdint-gcc.h>
#include <stdlib.h>
#include <stdbool.h>

#define MEM_LENGTH 256
#define PROG_MEM_LOC 0x80

typedef enum  {
	NoAddress,
	Immediate,
	Memory
} ADRESS_MODE;

typedef enum  {
	NOP, //execute nothing
	LDR, //load following register with following
	STR, //store following register at following mem
	ADD, //add b to a, store in a
	SUB, //add a to b, store in b
} INSTRUCTION_TYPE;

typedef struct{
	INSTRUCTION_TYPE type;
	char name[10];
	char label[50];
	void (*operate )(void);
	uint8_t (*addrmode)(void);
} INSTRUCTION;

extern INSTRUCTION instructions[16];

typedef struct{

	//program counter
		uint8_t PCO;
		
	//current opcode
		uint8_t OPC;
		
	//current address mode
		ADRESS_MODE ADM;
		
	//remaining cycles
		uint8_t CRE;

	//internal registers, 8bits
		uint8_t IR1;
		uint8_t IR2;
	
	//addressable registers, 8 bits
		uint8_t AR0;
		uint8_t AR1;
		uint8_t AR2;
	
	//ROM + RAM
		uint8_t RAM[MEM_LENGTH];
	
	//Carry flag
		bool C;
	
	//Zero Flag
		bool Z;
	
	//Signed arithmetic flag
		bool S;
	
	//current instruction
		INSTRUCTION INS;
	
	
} CPU;

extern CPU cpu;

//Address modes
	uint8_t cpu_am_AMN();
	uint8_t cpu_am_AMI();
	uint8_t cpu_am_AMM();
	
//Instructions
	void cpu_ins_NOP();
	void cpu_ins_LRR();
	void cpu_ins_STR();
	
	void system_restart();
	void cpu_execute();
