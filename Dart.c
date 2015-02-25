//
//  Dart.c
//  DART4iOS
//
//  Created by Will Stafford on 5/28/14.
//  Copyright (c) 2014 iWill LLC. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "Dart.h"
#include <sys/stat.h>

int dartInit() {
	
	if (createRam()) {
		regsSym = dart_sym("R"); // Registers
		programCounterSym = dart_sym("PC"); // Program Counter
		callStackSym = dart_sym("CS"); // Callstack
		compareFlagsSym = dart_sym("CF"); // Compare Flags
		callStackIndexSym = dart_sym("CSI"); // Callstack Index
		regs = &getSymbol(regsSym);
		programCounter = &getSymbol(programCounterSym);
		callstack = &getSymbol(callStackSym);
		callstackIndex = &getSymbol(callStackIndexSym);
		compareFlags = &getSymbol(compareFlagsSym);
		
	} else {
		return GENERAL_ERROR;
	}
	load_commands();
	
    int z;
	for (z = 1; z < 0xFF; z++) {
		regs[z] = regs[0]+z;
	}
	dartInitiated = 1;
	dart_kill_exe = 0;
	return 1;
}

void dartKillExecution() {
	dart_kill_exe = 1;
}

off_t dart_fsize(const char *filename) {
	struct stat st;
	
	if (stat(filename, &st) == 0)
		return st.st_size;
	
	return -1;
}

void convertUnitRangeToBytes(char *output, unit pointer, unit size) {
	int x;
	for (x = 0; x < size; x++) {
		output[x] = (char)(ram[pointer+x]);
	}
}

void convertUnitStringToUTF8(char *output, unit pointer) {
    output[0] = '\0';
	do {
		sprintf(output, "%s%c", output, (char)ram[pointer]);
		pointer++;
	} while (ram[pointer] != '\0');
}

unit unitStringLength(unit pointer) {
    int x = 0;
    while (ram[pointer+x] != '\0') {
        x++;
    }
    return x;
}

map_symbol dart_map(char *name, unit size) {
	mapped[mapIndex].name = name;
	mapped[mapIndex].size = size;
	mapped[mapIndex].idcode = (0x01+mapIndex)<<16;
	mapped[mapIndex].address = -1;
	mapIndex++;
	return mapped[mapIndex-1];
}

unit dart_mapped_index(char *name) {
	int x;
	for (x = 0; x < mapIndex; x++) {
		if (!strcasecmp(name, mapped[x].name)) {
			return x;
		}
	}
	return -1;
}

void dart_load_maps() {
	if (mapsSet) {
		return;
	}
	dart_map("R", 256); // Registers
	dart_map("PC", 1); // Program Counter
	dart_map("CS", 512); // Callstack
	dart_map("CF", 512); // Compare Flags
	dart_map("CSI", 1); // Callstack Index
	dart_map("ARG", 64); // For arguments
	dart_map("XARG", 64); // For external arguments (outside the VM)
	dart_map("RETV", 1); // For returning values
	dart_map("XAC", 1); // XARG count
	mapsSet = 1;
	
}

map_symbol dart_sym(char *name) {
	if (!mapsSet) {
		printf("Maps not set!\n");
	}
	unit idx = dart_mapped_index(name);
	return mapped[idx];
}

unit dart_sym_val(char *name, unit offset) {
	return ram[dart_sym(name).address];
}

new_cmd_block(MOV){
	ram[args[0]] = args[1];
	return NO_ERROR;
}

new_cmd_block(RET) {
	*callstackIndex = (*callstackIndex)-1;
	if (*callstackIndex < 0) {
		return EXIT;
	} else {
		*programCounter = callstack[*callstackIndex];
	}
	return NO_ERROR;
}

new_cmd_block(PUSH) {
	ram[stackIdx] = args[0];
	stackIdx++;
	return NO_ERROR;
}

new_cmd_block(POP) {
	stackIdx--;
	ram[args[0]] = ram[stackIdx];
	return NO_ERROR;
}

new_cmd_block(ADD) {
	ram[args[0]] += args[1];
	return NO_ERROR;
}

new_cmd_block(SUB) {
	ram[args[0]] -= args[1];
	return NO_ERROR;
}

new_cmd_block(MULT) {
	ram[args[0]] *= args[1];
	return NO_ERROR;
}

new_cmd_block(DIV) {
	ram[args[0]] /= args[1];
	return NO_ERROR;
}


new_cmd_block(MOD) {
	ram[args[0]] %= args[1];
	return NO_ERROR;
}

new_cmd_block(AND) {
	ram[args[0]] &= args[1];
	return NO_ERROR;
}

new_cmd_block(NOT) {
	ram[args[0]] += ~args[1];
	return NO_ERROR;
}

new_cmd_block(OR) {
	ram[args[0]] |= args[1];
	return NO_ERROR;
}

new_cmd_block(XOR) {
	ram[args[0]] ^= args[1];
	return NO_ERROR;
}

new_cmd_block(CMP) {
	compareFlags[*callstackIndex] = args[0] - args[1];
	return NO_ERROR;
}

new_cmd_block(JMP) {
	*programCounter = args[0];
	//printf("JMP: %lld", addressForNextExec);
	return NO_ERROR;
}

new_cmd_block(JE) {
	if (compareFlags[*callstackIndex] == 0) {
		return JMP(args, types);
	}
	return NO_ERROR;
}

new_cmd_block(JNE) {
	if (compareFlags[*callstackIndex] != 0) {
		return JMP(args, types);
	}
	return NO_ERROR;
}

new_cmd_block(JG) {
	if (compareFlags[*callstackIndex] > 0) {
		return JMP(args, types);
	}
	return NO_ERROR;
}

new_cmd_block(JGE) {
	if (compareFlags[*callstackIndex] >= 0) {
		return JMP(args, types);
	}
	return NO_ERROR;
}

new_cmd_block(JL) {
	if (compareFlags[*callstackIndex] < 0) {
		return JMP(args, types);
	}
	return NO_ERROR;
}

new_cmd_block(JLE) {
	if (compareFlags[*callstackIndex] <= 0) {
		return JMP(args, types);
	}
	return NO_ERROR;
}

new_cmd_block(CALL) {
	callstack[*callstackIndex] = *programCounter;
	*callstackIndex = (*callstackIndex)+1;
	JMP(args, types);
	return NO_ERROR;
}

new_cmd_block(NOP) {
	return NO_ERROR;
}

new_cmd_block(ALLC) {
	unit addr;
	addr = heap_alloc(args[1]);
	if (addr == -1) {
		return OUT_OF_MEMORY_ERROR;
	} else {
		ram[args[0]] = addr;
	}
	return NO_ERROR;
}

new_cmd_block(FREE) {
	sector_free(args[0]);
	return NO_ERROR;
}

new_cmd_block(PRINT) {
	unit start = 0;
	start = args[0];
	unit len = unitStringLength(start);
	char output[len+1];
	convertUnitStringToUTF8(output, start);
	//printf("%s\n", output);
	dart_sendOutputToDelegate(output);
	
	return NO_ERROR;
}

new_cmd_block(PRINTD) {
	char output[64];
	sprintf(output, "%d", args[0]);
	dart_sendOutputToDelegate(output);
	return NO_ERROR;
}

new_cmd_block(PRINTC) {
	char output[8];
	sprintf(output, "%c", (char)(args[0]));
	dart_sendOutputToDelegate(output);
	return NO_ERROR;
}

new_cmd_block(BREAK) {
	char output[128];
	sprintf(output, "EXEC:%d ARG1:%d ARG2:%d TYPE1:%x TYPE2:%x\n", *programCounter, args[0], args[1], types[0], types[1]);
	dart_sendOutputToDelegate(output);
	return NO_ERROR;
}

new_cmd_block(RSHFT) {
	ram[args[0]] >>= args[1];
	return NO_ERROR;
}

new_cmd_block(LSHFT) {
	ram[args[0]] <<= args[1];
	return NO_ERROR;
}

new_cmd_block(INS) {
	dart_requestInputFromDelegate();
	unsigned long length = strlen(dart_Input_Pointer);
	int x;
	for (x = 0; x <= length; x++) {
		ram[args[0]+x] = ((char *)dart_Input_Pointer)[x];
	}
	return NO_ERROR;
}

new_cmd_block(SRAND) {
	srand((unsigned int)args[0]);
	return NO_ERROR;
}

new_cmd_block(RAND) {
	ram[args[0]] = rand();
	return NO_ERROR;
}

new_cmd_block(TIME) {
	ram[args[0]] = (int)time(NULL);
	return NO_ERROR;
}

new_cmd_block(FOPEN) { // arg0 = addr to write to. arg1 = filename
    char filename[512];
    convertUnitStringToUTF8(filename, args[1]);
    FILE *f = fopen(filename, "rb");
    if (!f) {
        return GENERAL_ERROR;
    }
	
	long fsize = dart_fsize(filename);
    
    char *string = malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);
    
    string[fsize] = 0;
    writeUTF8StringToAddress(string, args[0]);
    free(string);
    fclose(f);
    return NO_ERROR;
}

new_cmd_block(FSAVES) {
    char filename[512];
    convertUnitStringToUTF8(filename, args[0]);
    char *contents = malloc(unitStringLength(args[1]));
    convertUnitStringToUTF8(contents, args[1]);
    FILE *f = fopen(filename, "w");
    if (!f) {
        return GENERAL_ERROR;
    }
    fprintf(f, "%s", contents);
    fclose(f);
    free(contents);
    return NO_ERROR;
}

new_cmd_block(FDELETE) {
    char filename[512];
    convertUnitStringToUTF8(filename, args[0]);
    remove(filename);
    return NO_ERROR;
}

new_cmd_block(STRLEN) {
    ram[args[0]] = unitStringLength(args[1]);
    return NO_ERROR;
}

new_cmd_block(COMPILE) {
	unit size = unitStringLength(args[1]);
#warning This is an awful allocation.
    unit *code = malloc(size*sizeof(unit));
	
    char buffer[size+1];
    convertUnitStringToUTF8(buffer, args[1]);
	//printf("%s\n", buffer);
	unit realSize = compileScriptToArray(buffer, &code);
    unit startAddress = loadBinary(code, realSize);
    if (startAddress == OUT_OF_MEMORY_ERROR) {
        return OUT_OF_MEMORY_ERROR;
    }
    ram[args[0]] = startAddress;
	//printf("R2: %d, should be: %d\n", regs[2], ram[args[0]]);
    free(code);
    //executeCode(startAddress);
    return NO_ERROR;
}

new_cmd_block(FSAVE) {
	char filename[512];
	convertUnitStringToUTF8(filename, args[0]);
	unit ptr = ram[args[1]];
	unit size = ram[args[1]+1];
	
	char *contents = malloc(size);
	convertUnitRangeToBytes(contents, ptr, size);
	FILE *f = fopen(filename, "w");
	if (!f) {
		return GENERAL_ERROR;
	}
	fwrite(contents, sizeof(char), size, f);
	fclose(f);
	free(contents);
	return NO_ERROR;
}

new_cmd_block(FSIZE) {
	char filename[512];
	convertUnitStringToUTF8(filename, args[1]);
	ram[args[0]] = (unit)dart_fsize(filename);
	return NO_ERROR;
}

void load_commands() {
	unit dart_bootloaderinit[] = {
		13, 0, 0, 589840, 2, 20, 44, 0, 130, 1, 0, 0, 0, 65616, 458768, 42, 2, 0, 65616, 65552, 23, 1, 2, 65616, 65552, 36, 1, 0, 65552, 65552, 40, 2, 1, 65616, 65552, 21, 2, 0, 65552, 1, 0, 0, 0, 524368, 2, 14, 24, 0, 130, 1, 29, -1, 0, 130, 1, 25, 24, 0, 136, 1, 0, 0, 1, 524368, 2, 14, 4, 0, 130, 1, 29, -1, 0, 130, 1, 1, 0, 0, 1, 1, 78, 111, 32, 97, 114, 103, 117, 109, 101, 110, 116, 115, 46, 46, 46, 10, 0
	};
	dart_bootloader = dart_bootloaderinit;
	cmdCount = 0;
	add_command("MOV", MOV);
	add_command("RET", RET);
	add_command("PUSH", PUSH);
	add_command("POP", POP);
	add_command("ADD", ADD);
	add_command("SUB", SUB);
	add_command("MULT", MULT);
	add_command("DIV", DIV);
	add_command("MOD", MOD);
	add_command("AND", AND);
	add_command("NOT", NOT);
	add_command("OR", OR);
	add_command("XOR", XOR);
	add_command("CMP", CMP);
	add_command("JMP", JMP);
	add_command("JE", JE);
	add_command("JNE", JNE);
	add_command("JG", JG);
	add_command("JGE", JGE);
	add_command("JL", JL);
	add_command("JLE", JLE);
	add_command("CALL", CALL);
	add_command("NOP", NOP);
	add_command("ALLC", ALLC);
	add_command("FREE", FREE);
	add_command("PRINT", PRINT);
	add_command("PRINTD", PRINTD);
	add_command("PRINTC", PRINTC);
	add_command("BREAK", BREAK);
	add_command(":", NOP);
	add_command("RSHFT", RSHFT);
	add_command("LSHFT", LSHFT);
	add_command("INS", INS);
	add_command("SRAND", SRAND);
	add_command("RAND", RAND);
	add_command("TIME", TIME);
    add_command("FOPEN", FOPEN);
    add_command("FSAVES", FSAVES);
    add_command("FDELETE", FDELETE);
    add_command("STRLEN", STRLEN);
    add_command("COMPILE", COMPILE);
	add_command("FSAVE", FSAVE);
	add_command("FSIZE", FSIZE);
    dart_load_maps();
}

