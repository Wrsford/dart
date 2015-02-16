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

dart_new_input_handler(maininput) {
    return "hi";
}

dart_new_output_handler(mainoutput) {
    printf("%s", output);
}

dart_new_error_handler(mainerror) {
    printf("dart error: %d\n", retval);
}

dart_new_finish_handler(mainfinish) {
    //printf("finished running\n");
}

int main(int argc, const char * argv[])
{
	const char *filename;
	if (argc > 1) {
		filename = argv[1];
	} else {
		filename = "/Users/wrsford/Dropbox/Development/dart/libdartTest/test.dart1";
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
	
	
	
	dart_delegate myDelegate = dartDelegateWithInOutErrFinishObj(maininput, mainoutput, mainerror, mainfinish, NULL);
	/*myDelegate.objectRef = (void*)0xDEADFACE;
	myDelegate.inputHandler = inputHandler;
	myDelegate.outputHandler = outputHandler;
	myDelegate.finishedHandler = finishedHandler;*/
	dart_setDelegate(myDelegate);
	unit *code = malloc(2048);
    unit codeLen = compileScriptToArray(buffer, &code);
    if (!dartInitiated) {
		dartInit();
        dartInitiated = 1;
	}
	unit startAddress = loadBinary(code, codeLen);
    if (startAddress == OUT_OF_MEMORY_ERROR) {
        printf("Cannot load code, out of memory.\n");
    }
    executeCode(startAddress);
	free(code);
    return 0;
}

