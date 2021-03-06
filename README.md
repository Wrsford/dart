Readme
=============
>DART is a scripting language. It is intended for me, it will probably not be maintained properly, but it will most likely be further improved.  
  
>It works similarly to x86/ARM assembly, however it is much more portable. I made this because I wanted to practice assembly coding, but I didn't want to figure out how to setup a VM to run bare asm code. Also, making my own operating system was not my goal, my goal was to be able to practice assembly and use those skills in reverse engineering.  
  
>The scripts are psuedo-compiled, loaded into simulated RAM, and interpreted in a similar fashion to a CPU. I can't honestly call it a VM because that might mislead people into thinking it actually executes compiled code directly on the real CPU.  
  
>The simulated RAM has no simulated protection, allowing one to dynamically write new code to RAM and execute it. You can also overwrite the existing binary in memory and the code can self-mutate. It even has a command for compiling and loading more scripts in runtime!  
  
>I named it DART when I began to design the language, which was before I knew of the web language "Dart." Meh... mine's capitalized.

To Build:

	cd /path/to/dart/folder
	gcc -o dart *.c

or install it system-wide:

	cd /path/to/dart/folder
	sudo gcc -o /usr/bin/dart *.c

Example Usage:

	dart ~/path/to/file

Example Script (Fibonacci):

	MOV @R0, 0 ; R0 is the previous previous number
	MOV @R1, 1 ; R1 is the previous number
	MOV @R2, 1 ; R2 is the current number
	MOV @R3, 0 ; R3 is the counter
	MOV @R4, 20 ; Print 20 numbers
	PRINTD R1
	PRINTC ' '
	: LOOP:
	MOV @R2, R1
	ADD @R2, R0
	MOV @R0, R1
	MOV @R1, R2
	PRINTD R2
	PRINTC ' '
	ADD @R3, 1
	CMP R3, R4
	JL LOOP:
	PRINT "\nFinished!\n"
	RET

Output:

	1 1 2 3 5 8 13 21 34 55 89 144 233 377 610 987 1597 2584 4181 6765 10946 
	Finished!

**This could very much delete all your files if used improperly (or properly). I am not responsible if this damages your computer.**

Copyright Will Stafford 2015. All rights reserved.


Commands
=============

MOV
-------------
######MOV *dest*, *val*
>Places *val* into *dest*.

RET
-------------
######RET
>Returns, popping the callstack into PC.  
>If the callstack only contains one address, it will send an exit signal.

PUSH
-------------
######PUSH *val*
>Pushes *val* to the stack.

POP
-------------
######POP *dest*
>Pops the top value from the stack into *dest*.

ADD
-------------
######ADD *dest*, *val*
>Places (\**dest* + *val*) into *dest*.

SUB
-------------
######SUB *dest*, *val*
>Places (\**dest* + *val*) into *dest*.

MULT
-------------
######MULT *dest*, *val*
>Places (\**dest* * *val*) into *dest*.

DIV
-------------
######DIV *dest*, *val*
>Places (\**dest* / *val*) into *dest*.

MOD
-------------
######MOD *dest*, *val*
>Places (\**dest* % *val*) into *dest*.

AND
-------------
######AND *dest*, *val*
>Places (\**dest* & *val*) into *dest*.

NOT
-------------
######NOT  *dest*, *val*
>Places (\**dest* ! *val*) into *dest*.

OR
-------------
######OR *dest*, *val*
>Places (\**dest* | *val*) into *dest*.

XOR
-------------
######XOR *dest*, *val*
>Places (\**dest* ^ *val*) into *dest*.

CMP
-------------
######CMP *first*, *second*
>Compares *first* to *second* and sets compareFlag to it.

JMP
-------------
######JMP *dest*
>Sets PC to *dest*.

JE
-------------
######JE *dest*
>Sets PC to *dest* if compareFlag == 0.

JNE
-------------
######JNE *dest*
>Sets PC to *dest* if compareFlag != 0.

JG
-------------
######JG *dest*
>Sets PC to *dest* if compareFlag > 0.

JGE
-------------
######JGE *dest*
>Sets PC to *dest* if compareFlag >= 0.

JL
-------------
######JL *dest*
>Sets PC to *dest* if compareFlag < 0.

JLE
-------------
######JLE *dest*
>Sets PC to *dest* if compareFlag <= 0.

CALL
-------------
######CALL *dest*
>Pushes PC to the callstack and sets PC to *dest*.

NOP
-------------
######NOP
>No operation is performed, but it does use an execution cycle.

ALLC
-------------
######ALLC *dest*, *sz*
>Allocates *sz* bytes and places the address into *dest*.

FREE
-------------
######FREE *ptr*
>Frees allocated bytes at *ptr*.

PRINT
-------------
######PRINT *ptr*
>Prints the string at *ptr* (NULL terminated).  

Example:

	PRINT "Hello, "  
	MOV 123, "World!"  
	PRINT 123

Output:

	Hello, World!


PRINTD
-------------
######PRINTD *val*
>Prints *val* as a decimal value.

PRINTC
-------------
######PRINT *val*
>Prints *val* as a ASCII character.

BREAK
-------------
######BREAK *arg1*, *arg2*
>Prints PC, *arg1*, *arg2*, typeof *arg1*, typeof *arg2*. Mainly used for debugging.

:
-------------
######: *label*:
>Creates *label* used for jumping/calling. This is used for the compiler, but is executed as a NOP command.

RSHFT
-------------
######RSHFT *dest*, *val*
>Places (\**dest* >> *val*) into *dest*.

LSHFT
-------------
######LSHFT *dest*, *val*
>Places (\**dest* << *val*) into *dest*.

INS
-------------
######INS *dest*
>Requests input from the delegate and places it into *dest*.

SRAND
-------------
######SRAND *sd*
>Seeds the PRNG with *sd*.

RAND
-------------
######RAND *dest*
>Places a pseudo-random number in RAM at *dest*.

TIME
-------------
######TIME *dest*
>Places the current system time in RAM at *dest*.

FOPEN
-------------
######FOPEN *dest*, *path*
>Places contents of file at *path* into RAM at *dest*.  
>Make sure to call ALLC with enough space for the file.

FSAVES
-------------
######FSAVES *path*, *ptr*
>Saves the string at *ptr* to the disk at *path*.

FDELETE
-------------
######FDELETE *path*
>Removes item at *path* from the disk.

STRLEN
-------------
######STRLEN *dest*, *ptr*
>Sets *dest* to the length of a NULL-terminated string in RAM at *ptr*.

COMPILE
-------------
######COMPILE *pfil*, *ptr*
>Compiles the DART script at *ptr* into binary and loads it into RAM.  
>The address in which the binary is loaded is not defined by the caller.  
>Fills RAM at *pfil* with the address where the binary was placed.

FSAVE
-------------
######FSAVE *path*, *ptr*
>Saves the bytes at \**ptr* (size = \*(*ptr*+1)) to the disk at *path*.  
Usage:  

	ALLC @r0, 2 ; Allocate 2 bytes to r0
	MOV @r1, r0
	ADD @r1, 1 ; Put r0+1 into r1
	ALLC @r0, 12
	MOV r0, "hello world" ; Point r0 to "hello world"
	ALLC r2, 30
	MOV r2, "~/path/to/file" ; Point r2 to the file's path
	MOV r1, 12
	FSAVE r0, r2 ; Saves 12 bytes (r0+1) starting
		; at r0 to ~/path/to/file (r2)

FSIZE
-------------
######FSIZE *dest*, *path*
>Sets *dest* to the size of the file at *path*.