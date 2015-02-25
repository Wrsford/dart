//
//  DartRam.c
//  DART4iOS
//
//  Created by Will Stafford on 5/28/14.
//  Copyright (c) 2014 iWill LLC. All rights reserved.
//

#include <stdio.h>
#include "Dart.h"

int createRam() {
	int x;
	for (x = 0; x < SECTOR_COUNT-1; x++) {
		sectors[x].offset = 0;
		sectors[x].size = 0;
	}
	stackIdx = sector_alloc(0xfff);
    for (x = 0; x < mapIndex; x++) {
        mapped[x].address = sector_alloc(mapped[x].size);
		if (mapped[x].address == -1) {
			dart_sendErrorToDelegate(OUT_OF_MEMORY_ERROR);
		}
    }
	if (ram) {
		return 1;
	}
	return 0;
}

unit sector_alloc(unit size) {
	unit x;
	for (x = 0; x < SECTOR_COUNT-1; x++) {
		unit availableOffset = -1;
		availableOffset = sectors[x].offset+sectors[x].size;
		if (sectors[x].size == 0) {
			if (x > 0) {
				availableOffset = sectors[x-1].offset+sectors[x-1].size;
			}
			if (sectors[x+1].size == 0 || sectors[x+1].offset > availableOffset+size) {
				sectors[x].offset = availableOffset;
				sectors[x].size = size;
				return availableOffset;
			}
		}
	}
	return -1;
}

void sector_free(unit address) {
	unit x;
	for (x = 0; x < SECTOR_COUNT-1; x++) {
		if (sectors[x].offset == address) {
			sectors[x].size = 0;
		}
	}
}

void writeUTF8StringToAddress(char *utf8, unit addr) {
    unit x = 0;
    while (utf8[x] != '\0') {
        ram[addr+x] = utf8[x];
        x++;
    }
}

void writeByteToAddress(byte arg1, unit addr) {
	ram[addr] = arg1;
}

byte readByteAtAddress(address addr) {
	return ram[addr];
}

void writeBytesWithLengthToAddress(unit *bytes, unit length, unit addr) {
	unsigned int itr = 0;
	while (itr < length) {
		ram[addr+itr] = bytes[itr];
		itr++;
	}
}

void readBytesWithLengthFromAddressIntoArray(unit length, unit addr, unit **arry) {
	unsigned int itr = 0;
	while (itr < length) {
		(*arry)[itr] = ram[addr+itr];
		itr++;
	}
}

unit loadXString(char *string) {
	unit addr = sector_alloc((unit)strlen(string)+1);
	writeUTF8StringToAddress(string, addr);
	return addr;
}


