#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


typedef struct {
    bool z;
    bool n;
    bool v;
    bool c;
} CPUFlags;

typedef struct {
    uint64_t registers[15];
    uint64_t pc; // r15
    uint64_t currentCycle;
    CPUFlags flags;
    uint8_t *ram;
} CPU;

typedef struct {
    const char *path;
    size_t memSize;
    uint64_t loadAddr;
    uint64_t entry;
    uint64_t maxCycles;
    bool step;
    bool trace;
    bool noTty;
    bool rawInput;
    bool verbose;
} EmuArgs;

typedef enum {
    OPC_ALU_R   = 0x00,
    OPC_ALU_I   = 0x01,
    OPC_LOAD    = 0x02,
    OPC_STORE   = 0x03,
    OPC_BRANCH  = 0x04,
    OPC_JMP     = 0x05,
    OPC_JI      = 0x06,
    OPC_LI      = 0x07,
} Opcode;

void runEmulator();

static inline uint64_t extractBitfieldU(uint64_t value, int end, int start) {
    int length = (end - start) + 1;
    uint64_t shifted = value >> start;
 
    if (length >= 64) {
        return shifted; /* full width requested, no masking needed */
    }
 
    uint64_t mask = (1ULL << length) - 1;
    return shifted & mask;
}
 

static inline int64_t extractBitfieldS(uint64_t value, int end, int start) {
    uint64_t unsigned_field = extractBitfieldU(value, end, start);
    int length = (end - start) + 1;
 
    if (length >= 64) {
        return (int64_t)unsigned_field;
    }
 
    uint64_t sign_bit = 1ULL << (length - 1);
    if (unsigned_field & sign_bit) {
        /* negative: extend with 1s above the field */
        uint64_t extend_mask = ~((1ULL << length) - 1);
        return (int64_t)(unsigned_field | extend_mask);
    }
    return (int64_t)unsigned_field;
}

extern CPU cpuState;
extern EmuArgs args;

