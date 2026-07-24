#include "opcodes/alu.h"
#include "emulator.h"
#include "helpers/aluOps.h"
#include "fault/faultCodes.h"
#include "opcodes/encodings.h"

#include <stdint.h>

/* OPC-ALU-R 0x0 */
FaultCode opcodeAluR(uint32_t instruction) {
	EXTRACT_R_TYPE(instruction);
	uint64_t scratch;
	
    switch ( funct4 ) {
        case 0x0: 	scratch = doArithAdd(reg1, reg2); 	break;
        case 0x1: 	scratch = doArithSub(reg1, reg2); 	break;
        case 0x2: 	scratch = doArithAddc(reg1, reg2); 	break;
        case 0x3: 	scratch = doArithSubb(reg1, reg2); 	break;
        case 0x4: 	scratch = doArithAnd(reg1, reg2); 	break;
        case 0x5: 	scratch = doArithOr(reg1, reg2); 		break;
        case 0x6: 	scratch = doArithXor(reg1, reg2); 	break;
        case 0x7: 	scratch = doArithSll(reg1, reg2); 	break;
        case 0x8: 	scratch = doArithSrl(reg1, reg2); 	break;
        case 0x9:  	scratch = doArithSra(reg1, reg2); 	break;
        case 0xA: 	scratch = doArithRor(reg1, reg2); 	break;
        case 0xB: 	scratch = doArithRol(reg1, reg2); 	break;
        case 0xC: 	doArithCmp(reg1, reg2); 				break;
        default:
            return FAULT_ILLEGAL_ENCODING; /* Trap */ break;
    }
	
	if (funct4 != 0xC && rd != 0xF) 
		cpuState.registers[rd] = scratch;

    cpuState.registers[15] += 4;
	
	return FAULT_NONE;
}

/* OPC-ALU-I */
FaultCode opcodeAluI(uint32_t instruction) {
	EXTRACT_I_TYPE(instruction);	
	uint64_t scratch;
	
    switch ( funct4 ) {
        case 0x0: 	scratch = doArithAdd(reg1, imm); 		break;
        case 0x1: 	scratch = doArithSub(reg1, imm); 		break;
        case 0x2: 	scratch = doArithAddc(reg1, imm); 	break;
        case 0x3: 	scratch = doArithSubb(reg1, imm); 	break;
        case 0x4: 	scratch = doArithAnd(reg1, imm); 		break;
        case 0x5: 	scratch = doArithOr(reg1, imm); 		break;
        case 0x6: 	scratch = doArithXor(reg1, imm); 		break;
        case 0x7: 	scratch = doArithSll(reg1, imm); 		break;
        case 0x8: 	scratch = doArithSrl(reg1, imm); 		break;
        case 0x9:  	scratch = doArithSra(reg1, imm); 		break;
        case 0xA: 	scratch = doArithRor(reg1, imm); 		break;
        case 0xB: 	scratch = doArithRol(reg1, imm); 		break;
        case 0xC: 	doArithCmp(reg1, imm); 				break;
        default:
            return FAULT_ILLEGAL_ENCODING; /* Trap */ break;
    }
	
	if (funct4 != 0xC && rd != 0xF) 
		cpuState.registers[rd] = scratch;

    cpuState.registers[15] += 4;
	
	return FAULT_NONE;
}
