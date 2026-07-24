#include "opcodes/mem.h"
#include "fault/faultCodes.h"
#include "emulator.h"
#include "memory/mmio.h"
#include "opcodes/encodings.h"

#include <stdint.h>

/* OPC-LOAD 0x2 */
FaultCode opcodeLoad(uint32_t instruction) {
	EXTRACT_I_TYPE(instruction);

	uint64_t addr = reg1 + imm;
	uint64_t scratch;

	int numBytes = 0;
	bool signedLoad = true;

	switch (funct4) {
		case 0x0: numBytes = 1; 	break;
		case 0x1: numBytes = 2;     break;
		case 0x2: numBytes = 4; 	break;
		case 0x3: numBytes = 8; 	break;
		case 0x4: numBytes = 1; signedLoad = false; 	break;
		case 0x5: numBytes = 2; signedLoad = false; 	break;
		case 0x6: numBytes = 4; signedLoad = false; 	break;
		default:
			return FAULT_ILLEGAL_ENCODING;
			break;
	}

	for (int i = 0; i < MMIO_ENTRIES; i++) {
		if (addr >= mmioEntries[i].base && addr < mmioEntries[i].base + mmioEntries[i].length) {
			cpuState.registers[15] += 4;
			return mmioEntries[i].readMmio(addr, numBytes, &scratch);
		}
	}

	if (addr + numBytes > args.memSize) 
        return FAULT_ILLEGAL_LOAD;
	
	scratch = 0;
    for (int i = 0; i < numBytes; i++) {
        scratch |= ((uint64_t)cpuState.ram[addr + i]) << (8 * i);
    }

	if (signedLoad)
		scratch = extractBitfieldS(scratch, numBytes * 8 - 1, 0);

	if (rd != 0xF) 
		cpuState.registers[rd] = scratch;

	cpuState.registers[15] += 4;

	return FAULT_NONE;
}

/* OPC-STORE, 0x3 */
FaultCode opcodeStore(uint32_t instruction) {
	EXTRACT_S_TYPE(instruction);

	uint64_t addr = reg1 + imm;

	int numBytes = 0;

	switch (funct4) {
		case 0x0: numBytes = 1; 	break;
		case 0x1: numBytes = 2;     break;
		case 0x2: numBytes = 4; 	break;
		case 0x3: numBytes = 8; 	break;
		default:
			return FAULT_ILLEGAL_ENCODING;
			break;
	}

	for (int i = 0; i < MMIO_ENTRIES; i++) {
		if (addr >= mmioEntries[i].base && addr < mmioEntries[i].base + mmioEntries[i].length) {
			cpuState.registers[15] += 4;
			return mmioEntries[i].writeMmio(addr, numBytes, reg2);
		}
	}

	if (addr + numBytes > args.memSize) 
        return FAULT_ILLEGAL_LOAD;
	
    for (int i = 0; i < numBytes; i++) {
        cpuState.ram[addr + i] = reg2 >> i * 8;
    }

	cpuState.registers[15] += 4;

	return FAULT_NONE;
}
