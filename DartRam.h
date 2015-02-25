//
//  DartRam.h
//  DART4iOS
//
//  Created by Will Stafford on 5/28/14.
//  Copyright (c) 2014 iWill LLC. All rights reserved.
//

typedef int unit;
//#define STACK_OFFSET 0xFFF
//#define HEAP_OFFSET 0xFFF+STACK_OFFSET
#ifndef DART4iOS_DartRam_h
#define DART4iOS_DartRam_h
#include "Dart.h"
#include <limits.h>

/*typedef enum {
	DART_RAM_BINARY = 0x1,
	DART_RAM_HEAP = 0x10,
} ramType;*/

typedef struct {
	unit offset;
	unit size;
} dart_ram_sector;

unit ram[0x3ffc];
//unit *heap;
//unit *stack;
unit stackIdx;
unit prgmoff;
#define SECTOR_COUNT 0xFFFF
dart_ram_sector sectors[SECTOR_COUNT];

// Basic Ops
int createRam();
unit sector_alloc(unit size);
#define binary_alloc(x) sector_alloc(x)
#define heap_alloc(x) sector_alloc(x)
void sector_free(unit address);
void writeUTF8StringToAddress(char *utf8, unit addr);
void writeByteToAddress(byte arg1, unit addr);
byte readByteAtAddress(address addr);
void writeBytesWithLengthToAddress(unit *bytes, unit length, unit addr);
void readBytesWithLengthFromAddressIntoArray(unit length, unit addr, unit **arry);

unit loadXString(char *string);
// Advanced Ops
//#define writeByteToHeapAddress(x, y) writeByteToAddress(x, y+HEAP_OFFSET)
//#define readByteFromHeapAddress(y) readByteFromAddress(y+HEAP_OFFSET)

#endif
