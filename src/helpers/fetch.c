#include "helpers/fetch.h"
#include "fault/faultCodes.h"
#include "memory/mmio.h"
#include "emulator.h"
#include <stdint.h>

FaultCode fetchInstruction(uint64_t addr, uint32_t* val) {

	uint32_t scratch;

	for (int i = 0; i < MMIO_ENTRIES; i++) {
		if (addr >= mmioEntries[i].base && addr < mmioEntries[i].base + mmioEntries[i].length) {
			return FAULT_ILLEGAL_FETCH; /* Cannot Fetch from MMIO Region */
		}
	}

	if (addr + 4 > args.memSize) 
        return FAULT_ILLEGAL_FETCH;
	
	scratch = 0;
    for (int i = 0; i < 4 /* Fetch 4 Bytes */; i++) {
        scratch |= ((uint32_t)cpuState.ram[addr + i]) << (8 * i);
    }

	*val = scratch;

	return FAULT_NONE;
}
