# 4C04 L-EOT Processor Emulator

4c04 L EOT Processor Emulator
The 4c04 is a fictional virtual CPU, complete with it's own assembly language.

It's a true 8-bit (byte) processor; it can only address memory locations 0-255 (256 bytes), and store 8 bits on one of four addressable registers.

The memory is laid out as follows:
* 0x00 - 0x6F is 112 bytes of none reserved RAM
* 0x70 - 0x7F is 16 bytes of reserved Stack memory
* 0x80 - 0xFF is 128 bytes of reserved ROM for program logic and data

This application mounts the 4c04 program through the use of an included assembler. Put your assembly in a file called "prog.txt" in the same directory as the executable, and that will be assembled into the ROM area of memory.

Details for the Assembly syntax can be found in draw.c

To compile, you must link the X11 libraries

On Linux:

	$ sudo apt-get install -y libx11-dev
		
The emulator requires an X11 enabled desktop environment.

On Linux:

    $ sudo apt install x11-apps
		
On Windows:
* Install XMing
* Install Putty
* Enable and Install Ubuntu for WSL (Windows Subsystem for Linux)
* Install X11 on Ubuntu
* Configure Putty to forward X11
* Launch via Putty from Ubuntu (./4c04)
