#pragma once

#include <stdint.h>

typedef struct {
    uint64_t registers[15];
    uint64_t pc; // r15
    uint8_t *ram;
} CPU;