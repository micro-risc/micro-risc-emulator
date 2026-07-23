#include "emulator.h"
#include "memory.h"
#include "aluOps.h"

#include <stdlib.h>
#include <stdio.h>

static void* dispatchTable[256];

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

    cpuState.pc = args.entry;

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

    uint32_t instruction;
    uint8_t funct4;
    uint8_t funct8;
    uint8_t rd;
    uint8_t rs1;
    uint8_t rs2;

    uint64_t reg1;
    uint64_t reg2;
    uint64_t imm;
    uint64_t scratch;
    bool we;
    
    /* The table of all the opcodes */
    for (int i = 0; i < 256; i++) {
        dispatchTable[i] = &&invalidOpcode; /* Zero out every entry */
    }

    dispatchTable[OPC_ALU_R]    = &&opcAluR;
    dispatchTable[OPC_ALU_I]    = &&opcAluI;
    dispatchTable[OPC_BRANCH]      = &&opcBranch;
    dispatchTable[OPC_JMP]      = &&opcJmp;
    dispatchTable[OPC_LI]       = &&opcLi;

    /* The actual emulator */

    stepBegin:
    if (readMem32(cpuState.pc, &instruction)) {
        fprintf(stderr, "Address out of range.");
        goto exitEmulator;
    }

    if (args.trace || args.verbose) {
        fprintf(tfptr, "Cycle: %ld,\tPC: 0x%016lx,\tInstruction: 0x%08x\n", cpuState.currentCycle, cpuState.pc, instruction);
    }

    if (args.verbose) {
        /* Dump Registers (Before Execution) */
        fprintf(tfptr, "a0: 0x%016lx,\ta1: 0x%016lx\ta2: 0x%016lx,\ta3: 0x%016lx\n", cpuState.registers[0], cpuState.registers[1], cpuState.registers[2], cpuState.registers[3]);
        fprintf(tfptr, "t0: 0x%016lx,\tt1: 0x%016lx\tt2: 0x%016lx,\ts0: 0x%016lx\n", cpuState.registers[4], cpuState.registers[5], cpuState.registers[6], cpuState.registers[7]);
        fprintf(tfptr, "s1: 0x%016lx,\ts2: 0x%016lx\ts3: 0x%016lx,\ts4: 0x%016lx\n", cpuState.registers[8], cpuState.registers[9], cpuState.registers[10], cpuState.registers[11]);
        fprintf(tfptr, "lr: 0x%016lx,\tfp: 0x%016lx\tsp: 0x%016lx,\tpc: 0x%016lx\n", cpuState.registers[12], cpuState.registers[13], cpuState.registers[14], cpuState.pc);

        /* Dump Flags (Before Execution) */
        fprintf(tfptr, "Flags: ");
        if (cpuState.flags.z) fprintf(tfptr, "Z"); else fprintf(tfptr, "z");
        if (cpuState.flags.n) fprintf(tfptr, "N"); else fprintf(tfptr, "n");
        if (cpuState.flags.v) fprintf(tfptr, "V"); else fprintf(tfptr, "v");
        if (cpuState.flags.c) fprintf(tfptr, "C"); else fprintf(tfptr, "c");

        fprintf(tfptr, "\n\n");
    }

    cpuState.currentCycle++;
    if (cpuState.currentCycle > args.maxCycles) {
        printf("%ld Cycles Finished\n", args.maxCycles);
        goto exitEmulator;
    }

    /* Extract funct4/8 and Reg select fields */
    funct4  = extractBitfieldU(instruction, 15, 12);
    funct8  = extractBitfieldU(instruction, 31, 24);
    rd      = extractBitfieldU(instruction, 11, 8);
    rs1     = extractBitfieldU(instruction, 19, 16);
    rs2     = extractBitfieldU(instruction, 23, 20);

    reg1 = (rs1 == 0xF) ? cpuState.pc : cpuState.registers[rs1];
    reg2 = (rs2 == 0xF) ? cpuState.pc : cpuState.registers[rs2];
    we = (rd != 0xF);

    /* Dispatch Instruction */
    goto *dispatchTable[(uint8_t) instruction];

    /* OPC-ALU-R, 0x00 */
    opcAluR:
    switch ( funct4 ) {
        case 0x0:
            cpuState.registers[rd] = (we) ? doArithAdd(reg1, reg2) : cpuState.registers[rd];
            break;
        case 0x1:
            cpuState.registers[rd] = (we) ? doArithSub(reg1, reg2) : cpuState.registers[rd];
            break;
        case 0x2:
            cpuState.registers[rd] = (we) ? doArithAddc(reg1, reg2) : cpuState.registers[rd];
            break;
        case 0x3:
            cpuState.registers[rd] = (we) ? doArithSubb(reg1, reg2) : cpuState.registers[rd];
            break;
        case 0x4:
            cpuState.registers[rd] = (we) ? doArithAnd(reg1, reg2) : cpuState.registers[rd];
            break;
        case 0x5:
            cpuState.registers[rd] = (we) ? doArithOr(reg1, reg2) : cpuState.registers[rd];
            break;
        case 0x6:
            cpuState.registers[rd] = (we) ? doArithXor(reg1, reg2) : cpuState.registers[rd];
            break;
        case 0x7:
            cpuState.registers[rd] = (we) ? doArithSll(reg1, reg2) : cpuState.registers[rd];
            break;
        case 0x8:
            cpuState.registers[rd] = (we) ? doArithSrl(reg1, reg2) : cpuState.registers[rd];
            break;
        case 0x9:
            cpuState.registers[rd] = (we) ? doArithSra(reg1, reg2) : cpuState.registers[rd];
            break;
        case 0xA:
            cpuState.registers[rd] = (we) ? doArithRor(reg1, reg2) : cpuState.registers[rd];
            break;
        case 0xB:
            cpuState.registers[rd] = (we) ? doArithRol(reg1, reg2) : cpuState.registers[rd];
            break;
        case 0xC:
            doArithCmp(reg1, reg2);
            break;
        default:
            goto invalidOpcode;
            break;
    }
    cpuState.pc += 4;
    goto stepBegin;

    /* OPC-ALU-I 0x1 */
    opcAluI:
    reg2 = extractBitfieldS((uint64_t) instruction, 31, 20);
    goto opcAluR; /* So we dont rewrite the switch statement again */

    /* OPC-BRANCH, 0x4 */
    opcBranch:
    imm = extractBitfieldS((uint64_t) instruction, 31, 16);
    scratch = extractBitfieldU((uint64_t) instruction, 11, 8);
    imm = (imm << 6) | (scratch << 2);
    switch ( funct4 ) {
        case 0x0: // bzs
            cpuState.pc += (cpuState.flags.z) ? imm : 4;
            break;
        case 0x1: // bzc
            cpuState.pc += (!cpuState.flags.z) ? imm : 4;
            break;
        case 0x2: // bns
            cpuState.pc += (cpuState.flags.n) ? imm : 4;
            break;
        case 0x3: // bnc
            cpuState.pc += (!cpuState.flags.n) ? imm : 4;
            break;
        case 0x4: // bvs
            cpuState.pc += (cpuState.flags.v) ? imm : 4;
            break;
        case 0x5: // bvc
            cpuState.pc += (!cpuState.flags.v) ? imm : 4;
            break;
        case 0x6: // bcs
            cpuState.pc += (cpuState.flags.c) ? imm : 4;
            break;
        case 0x7: // bcc
            cpuState.pc += (!cpuState.flags.c) ? imm : 4;
            break;
        case 0x8: // bgt (signed >)
            cpuState.pc += (!cpuState.flags.z && cpuState.flags.n == cpuState.flags.v) ? imm : 4;
            break;
        case 0x9: // bge (signed >=)
            cpuState.pc += (cpuState.flags.n == cpuState.flags.v) ? imm : 4;
            break;
        case 0xA: // blt (signed <)
            cpuState.pc += (cpuState.flags.n != cpuState.flags.v) ? imm : 4;
            break;
        case 0xB: // ble (signed <=)
            cpuState.pc += (cpuState.flags.z || cpuState.flags.n != cpuState.flags.v) ? imm : 4;
            break;
        case 0xC: // bhi (unsigned >)
            cpuState.pc += (cpuState.flags.c && !cpuState.flags.z) ? imm : 4;
            break;
        case 0xD: // bhs (unsigned >=)
            cpuState.pc += (cpuState.flags.c) ? imm : 4;
            break;
        case 0xE: // blo (unsigned <)
            cpuState.pc += (!cpuState.flags.c) ? imm : 4;
            break;
        case 0xF: // bls (unsigned <=)
            cpuState.pc += (!cpuState.flags.c || cpuState.flags.z) ? imm : 4;
            break;
        default: /* Impossible to reach */
            fprintf(stderr, "Emulator Has Reached an Invalid State");
            goto exitEmulator;
            break;
    }
    goto stepBegin;

    /* OPC-JMP, 0x05 */
    opcJmp:
    cpuState.pc = cpuState.pc + ( extractBitfieldS((uint64_t) instruction, 31, 8) << 2 ); 
    goto stepBegin;

    /* OPC-LI, 0x7 */
    opcLi:
    cpuState.registers[rd] = (we) ? extractBitfieldS((uint64_t) instruction, 31, 12) : cpuState.registers[rd];
    cpuState.pc += 4;
    goto stepBegin;

    /* Invalid Opcode / Funct Field */
    invalidOpcode:
    fprintf(stderr, "Invalid Opcode/Funct, Instruction: 0x%x, Address: 0x%lx\n", instruction, cpuState.pc);
    goto exitEmulator;

    exitEmulator:
    if (args.trace || args.verbose) 
        fclose(tfptr);

    free(cpuState.ram);
    exit(EXIT_FAILURE);
}