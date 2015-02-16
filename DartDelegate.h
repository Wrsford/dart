//
//  DartDelegate.h
//  DART4iOS
//
//  Created by Will Stafford on 6/5/14.
//  Copyright (c) 2014 iWill LLC. All rights reserved.
//

#ifndef DART4iOS_DartDelegate_h
#define DART4iOS_DartDelegate_h
#include "Dart.h"

/*#define dart_newDartOutputHandler(name) void name(char *output, void *objectRef)
#define dart_newDartFinishedHandler(name) void name(unit retcode, void *objectRef)
#define dart_newDartErrorHandler(name) void name(unit retcode, void *objectRef)
//#define dart_newDartGetStringInput(name) char* name(void *inputPointer, void *objectRef)
#define dart_newDartGetInput(name) void name(void *objectRef)

dart_newDartOutputHandler(sampleDartOutputHandler);
dart_newDartFinishedHandler(sampleDartFinishedHandler);
dart_newDartErrorHandler(sampleDartErrorHandler);
//dart_newDartGetStringInput(sampleDartGetStringInput);
dart_newDartGetInput(sampleDartGetInput);

typedef typeof(sampleDartOutputHandler) dartOutputHandler;
typedef typeof(sampleDartFinishedHandler) dartFinishedHandler;
typedef typeof(sampleDartErrorHandler) dartErrorHandler;
//typedef typeof(sampleDartGetStringInput) dartGetStringInput;
typedef typeof(sampleDartGetInput) dartGetInput;


typedef struct {
	dartOutputHandler *outputHandler;
	dartFinishedHandler *finishedHandler;
	dartErrorHandler *errorHandler;
	//dartGetStringInput *stringInputHandler; // todo
	dartGetInput *inputHandler; //todo
	void *objectRef;
} dart_delegated;

dart_delegate dartDelegate;*/
#define dart_setDelegate(name) dartDelegate = name

void dart_sendOutputToDelegate(char *output);
void dart_sendErrorToDelegate(int retCode);
void dart_sendFinishToDelegate(int retCode);
void dart_requestInputFromDelegate();



#define dart_new_input_handler(name) void* name(void *objRef)
dart_new_input_handler(dart_sample_input_handler);
typedef typeof(dart_sample_input_handler) dart_input_handler;

#define dart_new_output_handler(name) void name(void *objRef, char *output)
dart_new_output_handler(dart_sample_output_handler);
typedef typeof(dart_sample_output_handler) dart_output_handler;

#define dart_new_error_handler(name) void name(void *objRef, unit retval)
dart_new_error_handler(dart_sample_error_handler);
typedef typeof(dart_sample_error_handler) dart_error_handler;

#define dart_new_finish_handler(name) void name(void *objRef, unit retval)
dart_new_finish_handler(dart_sample_finish_handler);
typedef typeof(dart_sample_finish_handler) dart_finish_handler;

typedef struct {
	dart_input_handler *input;
	dart_output_handler *output;
	dart_error_handler *error;
    dart_finish_handler *finish;
	void *object; // Helpful for extending into objective languages
    int initialized;
} dart_delegate;

dart_delegate dartDelegateWithInOutErrFinishObj(dart_input_handler in, dart_output_handler out, dart_error_handler err, dart_finish_handler finish, void *obj);

#endif
