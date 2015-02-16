//
//  main.c
//  dartcli
//
//  Created by Will Stafford on 7/10/14.
//  Copyright (c) 2014 iWill LLC. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "Dart.h"
dart_newDartGetInput(inputHandler) {
	scanf("%[^\n]", dart_Input_Pointer);
}

dart_newDartOutputHandler(outputHandler) {
	printf("%s", output);
}

dart_newDartFinishedHandler(finishedHandler) {
	exit((int)retcode);
}

int main(int argc, const char * argv[])
{
	const char *filename;
	if (argc > 1) {
		filename = argv[1];
	} else {
		filename = "/Users/wrsford/Dropbox/Development/dartcli/dartcli/test.dart1";
		printf("No input file defined.\n");
		//exit(1);
	}
	
	char * buffer = 0;
	long length;
	FILE * f = fopen (filename, "rb");
	
	if (f)
	{
		fseek (f, 0, SEEK_END);
		length = ftell (f);
		fseek (f, 0, SEEK_SET);
		buffer = malloc (length);
		if (buffer)
		{
			fread (buffer, 1, length, f);
		}
		fclose (f);
	}
	
	
	
	dart_delegate myDelegate;
	myDelegate.objectRef = (void*)0xDEADFACE;
	myDelegate.inputHandler = inputHandler;
	myDelegate.outputHandler = outputHandler;
	myDelegate.finishedHandler = finishedHandler;
	dart_setDelegate(myDelegate);
	unit *code = malloc(2048);
	executeCode(code, compileScriptToArray(buffer, &code));
	free(code);
    return 0;
}

