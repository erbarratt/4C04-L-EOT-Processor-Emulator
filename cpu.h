#include <stdio.h>
#include <stdint-gcc.h>
#include <stdlib.h>

#define MEM_LENGTH 256
#define PROG_MEM_LOC 0x80;

typedef enum  {
	NoAddress,
	Immediate,
	Memory
} ADRESS_MODE;

typedef enum  {
	NOP, //execute nothing
	LDR, //load following register with following
	STR, //store following register at following mem
	ABA, //add b to a, store in a
	AAB, //add a to b, store in b
} INSTRUCTION_TYPE;

typedef struct{
	INSTRUCTION_TYPE type;
	char label[50];
	void (*operate )(void);
	uint8_t (*addrmode)(void);
} INSTRUCTION;

typedef struct{

	uint8_t IR1;
	uint8_t IR2;
	uint8_t PC;
	uint8_t R0;
	uint8_t R1;
	uint8_t R2;
	
	uint8_t RAM[MEM_LENGTH];
	
	INSTRUCTION INS;
	uint8_t OP;
	ADRESS_MODE AM;
	
	uint8_t remCycls;
	
} CPU;

extern CPU cpu;

//Address modes
	uint8_t cpu_am_AMN();
	uint8_t cpu_am_AMI();
	uint8_t cpu_am_AMM();
	
//Instructions
	void cpu_ins_NOP();
	void cpu_ins_LRR();
	
	void system_restart();
	void cpu_execute();
