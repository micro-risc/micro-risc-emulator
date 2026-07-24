#pragma once

#include "fault/faultCodes.h"
#include <stdint.h>

FaultCode opcodeJmp(uint32_t instruction);
FaultCode opcodeJi(uint32_t instruction);
