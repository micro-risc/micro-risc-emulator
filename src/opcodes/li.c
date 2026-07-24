#include "opcodes/li.h"
#include "emulator.h"
#include "fault/faultCodes.h"
#include "opcodes/encodings.h"

FaultCode opcodeLi(uint32_t instruction) {
	EXTRACT_LI_TYPE(instruction);

	if (rd != 0xF)
		cpuState.registers[rd] = imm;
	
	cpuState.registers[15] += 4;

	return FAULT_NONE;
}
