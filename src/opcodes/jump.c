#include "opcodes/jump.h"
#include "emulator.h"
#include "fault/faultCodes.h"
#include "opcodes/encodings.h"

#include <stdint.h>

FaultCode opcodeJmp(uint32_t instruction) {
	EXTRACT_J_TYPE(instruction);
	cpuState.registers[15] += imm;

	return FAULT_NONE;
}

FaultCode opcodeJi(uint32_t instruction) {
	EXTRACT_I_TYPE(instruction);

	if (funct4 != 0) 
		return FAULT_ILLEGAL_ENCODING;

	cpuState.registers[15] = reg1 + imm;

	return FAULT_NONE;
}
