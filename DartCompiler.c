//
//  DartCompiler.c
//  DART4iOS
//
//  Created by Will Stafford on 5/28/14.
//  Copyright (c) 2014 iWill LLC. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include "Dart.h"

unit compileScriptToArray(char *script, unit **arry) {
	load_commands();
	unit totalLen = 0;
	int stringcount = 0;
	char strings[1024][1024];
	int stringIndexes[1024];
	int itr = 0;
	int linecount = 1;
	unit *result = *arry;
	while (script[itr] != '\0') {
		if (script[itr] == '\n') {
			linecount++;
		}
		itr++;
	}
	itr = 0;
	
	char *scritr = (char *)script;
	int legacyLineNum[1024];
	char organized[1024][1024];
	int lLN = 1;
	
	char *prenames[1024];
	int prelinenum[1024];
	int prenameitr = 0;
	
	while (itr < linecount) {
		
		sscanf(scritr, "%[^\n]", organized[itr]);
		scritr += strlen(organized[itr]);
		
		int x;
		for (x = 0; organized[itr][x] != '\0'; x++) {
			if (organized[itr][x] == ';') {
				organized[itr][x] = '\0';
				break;
			}
		}
		
		if (organized[itr][0] == ':') {
			prenames[prenameitr] = organized[itr]+2;
			prelinenum[prenameitr] = itr;
			prenameitr++;
			//organized[itr][0] = '\0';
			//continue;
		}
		
		if (organized[itr][0] == '\0') {
			itr--;
		} else {
			legacyLineNum[itr] = lLN;
			//printf("LINE %d: %s\n", legacyLineNum[itr], organized[itr]);
		}
		
		itr++;
		lLN++;
		if (scritr[0] == '\0') {
			break;
		}
		
		scritr++;
		if (scritr[0] == '\n') {
			lLN++;
			scritr++;
		}
	}
	itr = 0;
	
	/*char corrected[512][512];
	int prgmoff = 0;
	// Determine strings
	while (itr) {
		
	}
	
	itr = prgmoff;
	while (itr < linecount+prgmoff) {
		corrected[itr] = organized[itr-prgmoff];
	}*/
	
	while (itr < linecount && organized[itr][0] != '\0') {
		//printf("%s\n", organized[itr]);
		int cmdIdx = -1;
		char curCmdHolder[0xFF];
		char *curCmd = curCmdHolder;
		sscanf(organized[itr], "%[^ ]", curCmd);
		if (curCmd[0] == '\t') {
			curCmd[0] = 'N';
			curCmd[1] = 'O';
			curCmd[2] = 'P';
			curCmd[3] = '\0';
		}
		int x;
		for (x = 0; x < MAX_COMMANDS && commands[x].created; x++) {
			if (!strcasecmp(curCmd, commands[x].name)) {
				cmdIdx = x;
				break;
			}
		}
		if (cmdIdx == -1) {
			//printf("Command not found: {%s} ", curCmd);
		} else {
			//printf("CMD%d: {%s} ", cmdIdx, curCmd);
		}
		
		char whitespaceCounter[16];
		
		char *postcmd = organized[itr]+strlen(curCmd);
		sscanf(postcmd, "%[ \t]", whitespaceCounter);
		postcmd += strlen(whitespaceCounter);
		//printf("ARGS: {%s}", postcmd);
		
		// Determine arg split;
		char args[2][1024];
		unit decArgs[2];
		argtype types[2];
		types[0] = 0x0;
		types[1] = 0x0;
		for (x = 0; x < 1024; x++) {
			args[0][x] = '\0';
			args[1][x] = '\0';
		}
		
		int escape = 0;
		int argidx = 0;
		int adjustedItr = 0;
		int openQ = 0;
		for (x = 0; postcmd[x] != '\0'; x++) {
			if (escape) {
				char toAdd = postcmd[x];
				switch (postcmd[x]) {
					case 't':
						toAdd = '\t';
						break;
					case 'n':
						toAdd = '\n';
						break;
					case '\"':
						toAdd = '\"';
						break;
					case '\'':
						toAdd = '\'';
						break;
					case 'v':
						toAdd = '\v';
						break;
					case '\\':
						toAdd = '\\';
						break;
						
					default:
						break;
				}
				args[argidx][x-adjustedItr] = toAdd;
				escape = 0;
				continue;
			}
			
			if (postcmd[x] == '\\') {
				escape = 1;
				adjustedItr++;
				continue;
			}
			
			if (postcmd[x] == '\"' || postcmd[x] == '\'') {
				types[argidx] |= ARG_STRING;
				if (openQ) {
					openQ = 0;
				} else {
					openQ = 1;
				}
				if (postcmd[x] == '\'') {
					types[argidx] |= ARG_CHAR;
				}
				//args[argidx][x-adjustedItr] = '\"';
				adjustedItr++;
				continue;
			}
			
			if (!openQ && (postcmd[x] == ' ' || postcmd[x] == '\t')) {
				adjustedItr++;
				continue;
			}
			
			if (postcmd[x] == ',' && !openQ) {
				adjustedItr = x+1;
				argidx++;
				continue;
			}
			
			if (!openQ && postcmd[x] == '@') {
				types[argidx] |= ARG_ADDRESS_OF_REG;
				adjustedItr++;
				continue;
			}
			
			if (!openQ && postcmd[x] == '*') {
				types[argidx] |= ARG_VALUE_AT_ADDRESS;
				adjustedItr++;
				continue;
			}
			
			
            // Check for maps
			
            // parse to the end of the arg
            int m;
            
            int curLen = 0;
            
            int longestMatch = 0;
            int matchIdx = 0;
            
            for (m = 0; m < mapIndex; m++) {
                
                unsigned long mLen = strlen(mapped[m].name);
                if (mLen < longestMatch) {
                    continue;
                }
                int i;
                for (i = 0; i < mLen && postcmd[x+i] != '\0'; i++) {
                    char mCmpStr[2][2];
                    mCmpStr[0][0] = mapped[m].name[i];
                    mCmpStr[1][0] = postcmd[x+i];
                    mCmpStr[0][1] = '\0';
                    mCmpStr[1][1] = '\0';
                    if (!strcasecmp(mCmpStr[0], mCmpStr[1])) {
                        if (postcmd[strlen(postcmd)-1] != ':' && !openQ) {
                            curLen++;
                            
                            continue;
                        }
                    }
                    curLen = 0;
                    break;
                }
                
                if (curLen > longestMatch) {
                    longestMatch = curLen;
                    matchIdx = m;
                }
                curLen = 0;

            }
            if (longestMatch && longestMatch >= strlen(mapped[matchIdx].name)) {
                types[argidx] |= ARG_REG | mapped[matchIdx].idcode;
                adjustedItr += longestMatch;
                longestMatch = 0;
            }
            // End maps check*/
			
			if (postcmd[x] == '\0') {
				postcmd[x] = '0';
			}
			args[argidx][x-adjustedItr] = postcmd[x];
		}
		
		for (x = 0; x < prenameitr; x++) {
			if (strcasecmp(prenames[x], args[0]) == 0) {
				//printf("%s -> %d\n", prenames[x], prelinenum[x]*DART_COMPILED_LINE_SIZE);
				sprintf(args[0], "%d", prelinenum[x]*DART_COMPILED_LINE_SIZE-totalLen-1); // PC relative now
				types[0] = ARG_RELATIVE | ARG_NUM;
			}
			if (strcasecmp(prenames[x], args[1]) == 0) {
				sprintf(args[1], "%d", prelinenum[x]*DART_COMPILED_LINE_SIZE-totalLen-2); // PC relative now
				types[1] = ARG_RELATIVE | ARG_NUM;
			}
		}
		
		
		//char *strings[2];
		
		for (x = 0; x < 2; x++) {
			if (types[x] == 0) {
				if (args[x][0] == '\0') {
					types[x] = ARG_NONE;
				} else {
					types[x] = ARG_NUM;
				}
			}
			
			if (types[x] & ARG_REG) {
				if (args[x][0] == '\0') {
					decArgs[x] = 0;
				} else {
					sscanf(args[x], "%d", &(decArgs[x]));
				}
			} else if (types[x] & ARG_NUM) {
				sscanf(args[x], "%d", &(decArgs[x]));
			} else if (types[x] & ARG_CHAR) {
				char scanto = 0;
				sscanf(args[x], "%c", &scanto);
				decArgs[x] = scanto;
			} else if (types[x] & ARG_STRING) {
				strcpy(strings[stringcount], args[x]);
				stringIndexes[stringcount] = itr*5+1+x;
				stringcount++;
				types[x] |= ARG_RELATIVE;
				decArgs[x] = 0x54321;
			}
			else if (types[x] & ARG_NONE) {
				decArgs[x] = 0x0;
			}
		}
		
		result[totalLen] = cmdIdx;
		result[totalLen+1] = decArgs[0];
		result[totalLen+2] = decArgs[1];
		result[totalLen+3] = types[0];
		result[totalLen+4] = types[1];
		totalLen += 5;
		
		
		//printf("%s {%s} {%s} -- %llx:%llx:%llx:%llx:%llx\n", curCmd, args[0], args[1], result[itr*5+0], result[itr*5+1], result[itr*5+2], result[itr*5+3], result[itr*5+4]);
		//printf("A1:{%s}Type:{%d} A2:{%s}Type:{%d}", args[0], types[0], args[1], types[1]);
		
		
		//printf("\n");
		itr++;
	}
	
	int x;
	for (x = 0; x < stringcount; x++) {
		unit curAddr = totalLen;
		unsigned long stop = strlen(strings[x]);
		int i;
		for (i = 0; i <= stop; i++) {
			result[totalLen] = strings[x][i];
			totalLen++;
		}
		//printf("%d -> %d\n", stringIndexes[x], curAddr-stringIndexes[x]);
		result[stringIndexes[x]] = curAddr-stringIndexes[x];
	}
	
	
	return totalLen;
}

char* compileBootloader(char *path) {
	FILE *f = fopen(path, "rb");
	if (!f) {
		return "Failed to open file...\n";
	}
	
	long fsize = dart_fsize(path);
	
	char *string = malloc(fsize + 1);
	fread(string, fsize, 1, f);
	fclose(f);
	
	string[fsize] = 0;
	fclose(f);
	unit *code = malloc(sizeof(unit)*1024);
	unit size = compileScriptToArray(string, &code);
	realloc(code, size);
	char *retMe = malloc(2048);
	sprintf(retMe, "unit dart_bootloader_init[%d] = { \n", size);
	int x;
	for (x = 0; x < size; x++) {
		//printf("%d, ", code[x]);
		if (x != size-1) {
			sprintf(retMe, "%s%d, ", retMe, code[x]);
		} else {
			sprintf(retMe, "%s%d };", retMe, code[x]);
		}
		
		if (x%5 == 4) {
			sprintf(retMe, "%s\n", retMe);
		}
	}
	
	string[0] = '\0';
	decompile(code, size, &string);
	printf("%s\n", string);
	free(string);
	free(code);
	return retMe;
}

void decompile(unit *code, unit length, char** output) { // Not tested
														 // Tested: seems to work alright, need to do logical AND to
														 //		figure out the arg types correctly... And do string stuff
	unit dataStart = length;
	int x;
	for (x = 0; x < length; x+=5) {
		unit *basicBlock = &code[x];
		if (basicBlock[0] < cmdCount) {
			dart_command command = commands[basicBlock[0]];
			unit args[2];
			argtype types[2];
			args[0] = basicBlock[1];
			args[1] = basicBlock[2];
			types[0] = basicBlock[3];
			types[1] = basicBlock[4];
			char *argStrings[2];
			char *typeStrings[2];
#define typeCase(name) case name:\
			typeStrings[y] = "" #name ""

			
			int y;
			for (y = 0; y < 2; y++) {
				switch (types[y]) {
						typeCase(ARG_NONE);
						break;
						typeCase(ARG_NUM);
						break;
						typeCase(ARG_CHAR);
						break;
						typeCase(ARG_STRING);
						break;
						typeCase(ARG_REG);
						break;
						typeCase(ARG_VALUE_AT_ADDRESS);
						break;
						typeCase(ARG_ADDRESS_OF_REG);
						break;
						typeCase(ARG_RELATIVE); // Assume a string
						args[y] = x+y+args[y];
						
						break;
						
					default:
						typeStrings[y] = "";
						break;
				}
			}
			sprintf(*output, "%s%s %d, %d | %s, %s\n", *output, command.name, args[0], args[1], typeStrings[0], typeStrings[1]);
		} else {
			// Not a command
		}
		
	}
	
	
	/*
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
	*/
}