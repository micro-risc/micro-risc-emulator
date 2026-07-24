#pragma once

#include "emulator.h"
#include "fault/faultCodes.h"
#include <stdint.h>

FaultCode opcodeAluR(uint32_t instruction);
FaultCode opcodeAluI(uint32_t instruction);
