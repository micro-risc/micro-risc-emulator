#pragma once
#include "fault/faultCodes.h"
#include <stdint.h>

/* FETCH 4 Byte Instruction */
FaultCode fetchInstruction(uint64_t addr, uint32_t* val);
