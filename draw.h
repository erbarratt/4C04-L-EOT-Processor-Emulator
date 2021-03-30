#include <stdio.h>
#include <stdint-gcc.h>
#include <string.h>

//colors for printing
	extern char ANSI_COLOR_RED[];
	extern char ANSI_COLOR_GREEN[];
	extern char ANSI_COLOR_YELLOW[];
	extern char ANSI_COLOR_BLUE[];
	extern char ANSI_COLOR_MAGENTA[];
	extern char ANSI_COLOR_CYAN[];
	extern char ANSI_COLOR_RESET[];

void draw_ram();
void draw_hex(uint32_t hex, size_t size, char * dest);