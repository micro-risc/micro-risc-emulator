#pragma once

#include "fault/faultCodes.h"
#include <stdint.h>

FaultCode opcodeLoad(uint32_t instruction);
FaultCode opcodeStore(uint32_t instruction);
