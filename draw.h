#include <stdio.h>
#include <stdint-gcc.h>
#include <string.h>
#include <X11/Xlib.h>

//colors for printing
	extern char ANSI_COLOR_RED[];
	extern char ANSI_COLOR_GREEN[];
	extern char ANSI_COLOR_YELLOW[];
	extern char ANSI_COLOR_BLUE[];
	extern char ANSI_COLOR_MAGENTA[];
	extern char ANSI_COLOR_CYAN[];
	extern char ANSI_COLOR_RESET[];

void draw_hex(uint32_t hex, size_t size, char * dest);
void draw_string(Display * display, Window window, GC gc, unsigned long color, int x, int y, char * str);
void draw_cpu(Display * display, Window window, GC gc);
void draw_ram(Display * display, Window window, GC gc);
void draw_all(Display * display, Window window, GC gc);

