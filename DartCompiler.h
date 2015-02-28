//
//  DartCompiler.h
//  DART4iOS
//
//  Created by Will Stafford on 5/28/14.
//  Copyright (c) 2014 iWill LLC. All rights reserved.
//

#define DART_COMPILED_LINE_SIZE 5
#ifndef DART4iOS_DartCompiler_h
#define DART4iOS_DartCompiler_h
unit compileScriptToArray(char *script, unit **arry); // Returns program size
char* compileBootloader(char *path);
void decompile(unit *code, unit length, char** output);
#endif
