//
//  DartDelegate.c
//  DART4iOS
//
//  Created by Will Stafford on 6/5/14.
//  Copyright (c) 2014 iWill LLC. All rights reserved.
//

#include <stdio.h>
#include "Dart.h"

dart_delegate dartDelegateWithInOutErrFinishObj(dart_input_handler in, dart_output_handler out, dart_error_handler err, dart_finish_handler finish, void *obj) {
    dart_delegate returnMe;
    returnMe.input = in;
    returnMe.output = out;
    returnMe.error = err;
    returnMe.finish = finish;
    returnMe.object = obj;
    returnMe.initialized = 1;
    return returnMe;
}

void dart_sendOutputToDelegate(char *output) {
	if (dartDelegate.initialized) {
		dartDelegate.output(dartDelegate.object, output);
	}
}

void dart_sendErrorToDelegate(int retCode) {
    if (dartDelegate.initialized) {
        dartDelegate.error(dartDelegate.object, retCode);
    }
}

void dart_sendFinishToDelegate(int retCode) {
	if (dartDelegate.initialized) {
		dartDelegate.finish(dartDelegate.object, retCode);
	}
}

void dart_requestInputFromDelegate() {
	if (dartDelegate.initialized) {
		dart_Input_Pointer = dartDelegate.input(dartDelegate.object);
	}
}