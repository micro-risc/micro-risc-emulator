#pragma once

#include "fault/faultCodes.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct {
	uint64_t base;
	uint64_t length;
	FaultCode (*readMmio)(uint64_t addr, int bytes, uint64_t* val);
	FaultCode (*writeMmio)(uint64_t addr, int bytes, uint64_t val);
} MmioEntry;


#define MMIO_ENTRIES 1
extern MmioEntry mmioEntries[1];
