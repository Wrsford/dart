//
//  DartExecutor.c
//  DART4iOS
//
//  Created by Will Stafford on 6/1/14.
//  Copyright (c) 2014 iWill LLC. All rights reserved.
//

#include <stdio.h>
#include <memory.h>
#include "Dart.h"

unit loadBinary(unit *code, unit totalLen) {
    unit test = sector_alloc(totalLen);
	//regs[0] = sector_alloc(0xFF);
	
	
	writeBytesWithLengthToAddress(code, totalLen, test);
    return test;
}


unit executeCode(unit startAddress) {
	*programCounter = startAddress;
    prgmoff = startAddress;
    //addressForNextExec += prgmoff;
	command_return_code retCode = NO_ERROR;
	while (!dart_kill_exe) {
		argtype types[2];
		unit execAddr = *programCounter;
		types[0] = (argtype)ram[execAddr+3];
		types[1] = (argtype)ram[execAddr+4];
		unit args[2];
		args[0] = ram[execAddr+1];
		args[1] = ram[execAddr+2];
		if (types[0] & ARG_RELATIVE) {
			args[0] += execAddr+1;
		} if (types[1] & ARG_RELATIVE) {
			args[1] += execAddr+2;
		}
		unit cmdidx = ram[execAddr];
		
		//printf("%s %lld, %lld -- %x, %x\n", commands[cmdidx].name, args[0], args[1], types[0], types[1]);
        int r;
        for (r = 0; r < 2; r++) {
            unit idcode = (types[r] & 0xff0000)>>16;
            if (types[r] & ARG_ADDRESS_OF_REG) {
                args[r] = mapped[idcode-1].address+args[r];
            } else if (types[r] & ARG_REG) {
                args[r] = ram[mapped[idcode-1].address+args[r]];
            }
        }
		
		//printf("%lld -- %llx:%llx:%llx:%x:%x\n", addressForNextExec, cmdidx, args[0], args[1], types[0], types[1]);
		
		if (types[0] & ARG_VALUE_AT_ADDRESS) {
			args[0] = ram[args[0]];
		}
		types[0] = ARG_NUM;
		
		if (types[1] & ARG_VALUE_AT_ADDRESS) {
			args[1] = ram[args[1]];
		}
		types[1] = ARG_NUM;
		
		*programCounter = execAddr+5;
        if (cmdidx < 0) {
            dartDelegate.error(NULL, GENERAL_ERROR);
        }
		//printf("\n%s %lld, %lld -- %x, %x\n", commands[cmdidx].name, args[0], args[1], types[0], types[1]);
		retCode = commands[cmdidx].exec(args, types);
        dart_kill_exe = retCode == EXIT;
        if (dart_kill_exe) {
            continue;
        }
        if (retCode != NO_ERROR) {
            dart_sendErrorToDelegate(retCode);
        }
	}
	if (dart_kill_exe) {
		dart_kill_exe = 0;
	}
	if (retCode == EXIT) {
		retCode = NO_ERROR;
	}
	sector_free(prgmoff);
	dart_sendFinishToDelegate(retCode);
	return retCode;
}