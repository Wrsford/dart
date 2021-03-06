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
	char *buffer = malloc(512);
	scanf("%[^\n]", buffer);
    return buffer;
}

dart_new_output_handler(mainoutput) {
    printf("%s", output);
}

dart_new_error_handler(mainerror) {
    printf("dart error: 0x%x\n", retval);
}

dart_new_finish_handler(mainfinish) {
    //printf("finished running\n");
}

int main(int argc, const char * argv[])
{
	dart_delegate myDelegate = dartDelegateWithInOutErrFinishObj(maininput, mainoutput, mainerror, mainfinish, NULL);
	dart_setDelegate(myDelegate);
	
	//printf("%s\n", compileBootloader("/Users/wrsford/Dropbox/Development/dart/libdartTest/boot.dart1"));
	const char *filename;
	if (argc > 1) {
		return bootFile(&argv[1], argc-1);
	} else {
		filename = "/Users/wrsford/Dropbox/Development/dart/libdartTest/test.dart1";
		printf("No input file defined.\n");
		return bootFile(&filename, 1);
		//exit(1);
	}
	
	
	
	
	
	
	
}

