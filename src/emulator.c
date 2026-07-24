#include "emulator.h"
#include "fault/faultCodes.h"

#include "opcodes/alu.h"
#include "opcodes/branch.h"
#include "opcodes/jump.h"
#include "opcodes/li.h"
#include "opcodes/mem.h"
#include "helpers/fetch.h"
#include "memory/mmio.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/* The CPU State */
CPU cpuState;

/* Emulator args */
EmuArgs args;

void runEmulator() {

    /* Setup the CPU State */
    cpuState.currentCycle = 0;
    cpuState.ram = (uint8_t*) malloc(args.memSize);
    if (args.loadAddr > args.memSize || args.entry > args.memSize) {
        fprintf(stderr, "Entry Point and Load address must be within ram.");
        exit(EXIT_FAILURE);
    }

    cpuState.registers[15] = args.entry;

    /* Load the File into RAM */
    FILE* fptr = fopen(args.path, "r");
    if (fptr == NULL) {
        fprintf(stderr, "Error opening file: %s\n", args.path);
        free(cpuState.ram);
        exit(EXIT_FAILURE);
    }

    /* Assuming the file has 1 hex byte per line */
    char line[8];
    uint64_t addr = args.loadAddr;
    while (fgets(line, sizeof(line), fptr)) {
        uint8_t byte;
        int success = sscanf(line, "%hhx", &byte);

        if (success != 1) {
            fprintf(stderr, "Error In Provided File.");
            free(cpuState.ram);
            fclose(fptr);
            exit(EXIT_FAILURE);
        }

        if (addr >= args.memSize) {
            fprintf(stderr, "Provided File too Large For Memory Amount");
            free(cpuState.ram);
            fclose(fptr);
            exit(EXIT_FAILURE);
        }

        cpuState.ram[addr] = byte;
        addr++;
    }

    fclose(fptr);

    /* Trace File */
    FILE* tfptr;
    if (args.trace || args.verbose) {
        tfptr = fopen("trace.txt", "w");
        if (tfptr == NULL) {
            fprintf(stderr, "Could Not Open trace.txt for Tracing.");
            free(cpuState.ram);
            exit(EXIT_FAILURE);
        }
    }

    /* The actual emulator */
	while (true) {
		uint32_t instruction;
		int trap = fetchInstruction(cpuState.registers[15], &instruction);
		
		printTrace(tfptr, instruction); /* Also exits if maxCycles has passed */

		/* Handle Illegal Mem Access */
		if (trap != FAULT_NONE) {
			fprintf(stderr, "Illegal Fetch At: 0x%lx", cpuState.registers[15]);
    		
			if (args.trace || args.verbose) 
        		fclose(tfptr);

    		free(cpuState.ram);
    		exit(EXIT_FAILURE);
		} 

		/* Dispatch Based on Opcode */
		switch ((uint8_t)instruction) {
			case OPC_ALU_R: 	trap = opcodeAluR(instruction); 	break;
			case OPC_ALU_I: 	trap = opcodeAluI(instruction); 	break;
			case OPC_LOAD: 		trap = opcodeLoad(instruction); 	break;
			case OPC_STORE: 	trap = opcodeStore(instruction); 	break;
			case OPC_BRANCH: 	trap = opcodeBranch(instruction); 	break;
			case OPC_JMP: 		trap = opcodeJmp(instruction); 		break;
			case OPC_JI: 		trap = opcodeJi(instruction); 		break;
			case OPC_LI: 		trap = opcodeLi(instruction); 		break;
			default: trap = FAULT_ILLEGAL_ENCODING; 				break;
		}

		if (trap != FAULT_NONE) {
			fprintf(stderr, "Illegal Encoding: 0x%x, At: 0x%lx", instruction, cpuState.registers[15]);
    		
			if (args.trace || args.verbose) 
        		fclose(tfptr);

    		free(cpuState.ram);
    		exit(EXIT_FAILURE);

		}

		/* Update Stuff like Terminal Screen */
		displayTerminal();
	}

	/* Exit Emulator (Should never reach here but) */
    if (args.trace || args.verbose) 
        fclose(tfptr);

    free(cpuState.ram);
    exit(EXIT_FAILURE);
}

void printTrace(FILE* tfptr, uint32_t instruction) {
    if (args.trace || args.verbose) {
        fprintf(tfptr, "Cycle: %ld,\tPC: 0x%016lx,\tInstruction: 0x%08x\n", cpuState.currentCycle, cpuState.registers[15], instruction);
    }

    if (args.verbose) {
        /* Dump Registers (Before Execution) */
        fprintf(tfptr, "a0: 0x%016lx,\ta1: 0x%016lx\ta2: 0x%016lx,\ta3: 0x%016lx\n", cpuState.registers[0], cpuState.registers[1], cpuState.registers[2], cpuState.registers[3]);
        fprintf(tfptr, "t0: 0x%016lx,\tt1: 0x%016lx\tt2: 0x%016lx,\ts0: 0x%016lx\n", cpuState.registers[4], cpuState.registers[5], cpuState.registers[6], cpuState.registers[7]);
        fprintf(tfptr, "s1: 0x%016lx,\ts2: 0x%016lx\ts3: 0x%016lx,\ts4: 0x%016lx\n", cpuState.registers[8], cpuState.registers[9], cpuState.registers[10], cpuState.registers[11]);
        fprintf(tfptr, "lr: 0x%016lx,\tfp: 0x%016lx\tsp: 0x%016lx,\tpc: 0x%016lx\n", cpuState.registers[12], cpuState.registers[13], cpuState.registers[14], cpuState.registers[15]);

        /* Dump Flags (Before Execution) */
        fprintf(tfptr, "Flags: ");
        if (cpuState.flags.z) fprintf(tfptr, "Z"); else fprintf(tfptr, "z");
        if (cpuState.flags.n) fprintf(tfptr, "N"); else fprintf(tfptr, "n");
        if (cpuState.flags.v) fprintf(tfptr, "V"); else fprintf(tfptr, "v");
        if (cpuState.flags.c) fprintf(tfptr, "C"); else fprintf(tfptr, "c");

        fprintf(tfptr, "\n\n");
    }

    cpuState.currentCycle++;
    if (cpuState.currentCycle > args.maxCycles && args.maxCycles != 0) {
        printf("%ld Cycles Finished\n", args.maxCycles);
        
		if (args.trace || args.verbose) 
       		fclose(tfptr);

    	free(cpuState.ram);
    	exit(EXIT_SUCCESS);
    }
}
