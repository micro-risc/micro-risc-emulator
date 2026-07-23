#pragma once
#include <stdint.h>

#include "emulator.h"

static inline int readMem(uint64_t addr, int numBytes, uint64_t *value) {
    if (addr + numBytes > args.memSize) /* Modify later for MMIO */
        return 1;

    if (numBytes > 8)
        numBytes = 8; /* Just clamp for now */

    *value = 0;
    for (int i = 0; i < numBytes; i++) {
        *value |= ((uint64_t)cpuState.ram[addr + i]) << (8 * i);
    }

    return 0;
}

static inline int writeMem(uint64_t addr, int numBytes, uint64_t value) {
    if (addr + numBytes > args.memSize) /* Modify later for MMIO */
        return 1;

    if (numBytes > 8)
        numBytes = 8; /* Just clamp for now */

    for (int i = 0; i < numBytes; i++) {
        cpuState.ram[addr + i] = ( value >> (i * 8) ) & 0xFF;
    }

    return 0;
}

static inline int readMem64(uint64_t addr, uint64_t *out) {
    uint64_t temp;
    int fault = readMem(addr, 8, &temp);
    *out = (uint64_t)temp;
    return fault;
}

static inline int readMem32(uint64_t addr, uint32_t *out) {
    uint64_t temp;
    int fault = readMem(addr, 4, &temp);
    *out = (uint32_t)temp;
    return fault;
}

static inline int readMem16(uint64_t addr, uint16_t *out) {
    uint64_t temp;
    int fault = readMem(addr, 2, &temp);
    *out = (uint16_t)temp;
    return fault;
}

static inline int readMem8(uint64_t addr, uint8_t *out) {
    uint64_t temp;
    int fault = readMem(addr, 1, &temp);
    *out = (uint8_t)temp;
    return fault;
}

static inline int writeMem64(uint64_t addr, uint64_t in) {
    int fault = writeMem(addr, 8, in);
    return fault;
}

static inline int writeMem32(uint64_t addr, uint32_t in) {
    int fault = writeMem(addr, 4, in);
    return fault;
}

static inline int writeMem16(uint64_t addr, uint16_t in) {
    int fault = writeMem(addr, 2, in);
    return fault;
}

static inline int writeMem8(uint64_t addr, uint8_t in) {
    int fault = writeMem(addr, 1, in);
    return fault;
}