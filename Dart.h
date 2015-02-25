//
//  Dart.h
//  DART4iOS
//
//  Created by Will Stafford on 5/28/14.
//  Copyright (c) 2014 iWill LLC. All rights reserved.
//
typedef unsigned char byte;
typedef long long address;

#define DART_VERSION 1
#define MAX_COMMANDS 512
#ifndef DART4iOS_Dart_h
#define DART4iOS_Dart_h
#include <string.h>
#include "DartRam.h"
#include "DartCompiler.h"
#include "DartExecutor.h"
#include "DartParser.h"
#include "DartDelegate.h"

typedef enum {
	NO_ERROR = 0x0,
	EXIT = 0x10,
	FATAL_ERROR = 0x100,
	CRITICAL_ERROR = 0x1000,
	GENERAL_ERROR = 0x10000,
	MEMORY_FAULT = 0x100000,
	OUT_OF_MEMORY_ERROR = -1,
} command_return_code;

typedef enum {
	ARG_NONE = 0x1,
	ARG_NUM = 0x2,
	ARG_CHAR = 0x4,
	ARG_STRING = 0x8,
	ARG_REG = 0x10,
	ARG_VALUE_AT_ADDRESS = 0x20,
	ARG_ADDRESS_OF_REG = 0x40,
	ARG_RELATIVE = 0x80,
    // Reserve 0xXX00 for future use
    // Start maps at 0x010000
} argtype;

#define new_cmd_block(name) command_return_code name(unit args[2], argtype types[2])
#define add_command(y, z) strcpy(commands[cmdCount].name, y); commands[cmdCount].exec = &z; commands[cmdCount].created = 1; cmdCount++
int cmdCount;
new_cmd_block(runCMD);
typedef typeof(runCMD) cmdBlock;
typedef struct {
	char name[64];
	cmdBlock *exec;
	int created;
} dart_command;
int dart_kill_exe;
int dartInitiated;
typedef struct {
	char *name;
	unit size;
	unit idcode;
	unit address;
} map_symbol;

dart_command commands[512];
map_symbol regsSym; // Reserve 0xFE & 0xFF for strings
map_symbol compareFlagsSym;

void dartKillExecution();
int dartInit();
void load_commands();
off_t dart_fsize(const char *filename);
void convertUnitRangeToBytes(char *output, unit pointer, unit size);
void convertUnitStringToUTF8(char output[512], unit pointer);
unit unitStringLength(unit pointer);

// Delegation Stuff
dart_delegate dartDelegate;
void *dart_Input_Pointer;



int mapsSet;
map_symbol mapped[256];
unit mapIndex;
map_symbol dart_map(char *name, unit size);
unit dart_mapped_index(char *name);
void dart_load_maps();
map_symbol dart_sym(char *name);
unit dart_sym_val(char *name, unit offset);
map_symbol programCounterSym;
map_symbol callStackSym;
map_symbol callStackIndexSym;

unit *callstackIndex;
unit *callstack;
unit *programCounter;
unit *regs;
unit *compareFlags;

#define dart_BL_size 97
unit *dart_bootloader;

#define setSymbol(symbol, value) ram[symbol.address] = value
#define setSymbolWithOffset(symbol, offset, value) ram[symbol.address+offset] = value
#define getSymbol(symbol) ram[symbol.address]
#define getSymbolWithOffset(symbol, offset) ram[symbol.address+offset]

#endif
