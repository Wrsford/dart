//
//  DartExecutor.h
//  DART4iOS
//
//  Created by Will Stafford on 5/28/14.
//  Copyright (c) 2014 iWill LLC. All rights reserved.
//

#ifndef DART4iOS_DartExecutor_h
#define DART4iOS_DartExecutor_h
#include "Dart.h"
int runByteCode(byte *code);
//unit addressForNextExec;


unit loadBinary(unit *code, unit totalLen);
unit executeCode(unit startAddress);
void setXARGS(char **argsPtr, unit count); // Takes strings
unit bootFile(const char **args, int count);
#endif
