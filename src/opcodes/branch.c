#include "opcodes/branch.h"
#include "fault/faultCodes.h"
#include "opcodes/encodings.h"
#include "emulator.h"
#include <stdint.h>

FaultCode opcodeBranch(uint32_t instruction) {
    EXTRACT_B_TYPE(instruction);

    switch ( funct4 ) {
        case 0x0: cpuState.registers[15] += (cpuState.flags.z) 	? imm : 4;											break; /* BZS */
        case 0x1: cpuState.registers[15] += (!cpuState.flags.z) 	? imm : 4; 										break; /* BZC */
        case 0x2: cpuState.registers[15] += (cpuState.flags.n) 	? imm : 4;											break; /* BNS */
        case 0x3: cpuState.registers[15] += (!cpuState.flags.n) 	? imm : 4; 										break; /* BNC */
        case 0x4: cpuState.registers[15] += (cpuState.flags.v) 	? imm : 4;											break; /* BVS */
        case 0x5: cpuState.registers[15] += (!cpuState.flags.v) 	? imm : 4; 										break; /* BVC */
        case 0x6: cpuState.registers[15] += (cpuState.flags.c) 	? imm : 4;											break; /* BCS */
        case 0x7: cpuState.registers[15] += (!cpuState.flags.c) 	? imm : 4; 										break; /* BCC */
        case 0x8: cpuState.registers[15] += (!cpuState.flags.z && cpuState.flags.n == cpuState.flags.v) ? imm : 4; 	break; /* BGT (signed >) 	*/
        case 0x9: cpuState.registers[15] += (cpuState.flags.n == cpuState.flags.v) ? imm : 4; 						break; /* BGE (signed >=) 	*/
        case 0xA: cpuState.registers[15] += (cpuState.flags.n != cpuState.flags.v) ? imm : 4; 						break; /* BLT (signed <) 	*/
        case 0xB: cpuState.registers[15] += (cpuState.flags.z || cpuState.flags.n != cpuState.flags.v) ? imm : 4; 	break; /* BLE (signed <=) 	*/
        case 0xC: cpuState.registers[15] += (cpuState.flags.c && !cpuState.flags.z) 					? imm : 4;	break; /* BHI (unsigned >)  */
        case 0xD: cpuState.registers[15] += (cpuState.flags.c) 	? imm : 4; 											break; /* BHS (unsigned >=) */
        case 0xE: cpuState.registers[15] += (!cpuState.flags.c) 	? imm : 4; 										break; /* BLO (unsigned <)  */
        case 0xF: cpuState.registers[15] += (!cpuState.flags.c || cpuState.flags.z) ? imm : 4; 						break; /* BLS (unsigned <=) */
        default: /* Impossible to reach */
            fprintf(stderr, "Emulator Has Reached an Invalid State");
            return FAULT_ILLEGAL_ENCODING;
            break;
    }

	return FAULT_NONE;
}
