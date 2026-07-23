#pragma once
#include "emulator.h"

/* Shared Flag Funcs */
static inline void setFlagsAddSub(uint64_t a, uint64_t b, uint64_t result, int is_sub) {
    cpuState.flags.z = (result == 0);
    cpuState.flags.n = (result >> 63) & 1;

    if (is_sub) {
        cpuState.flags.c = (a >= b);                              /* no borrow */
        cpuState.flags.v = ((a ^ b) & (a ^ result)) >> 63 & 1;     /* signed overflow */
    } else {
        cpuState.flags.c = (result < a);                          /* unsigned wraparound */
        cpuState.flags.v = (~(a ^ b) & (a ^ result)) >> 63 & 1;    /* signed overflow */
    }
}

/* and/or/xor */
static inline void setFlagsLogic(uint64_t result) {
    cpuState.flags.z = (result == 0);
    cpuState.flags.n = (result >> 63) & 1;
}

/* Shifts */
static inline void setFlagsShift(uint64_t result, uint64_t carry_out) {
    cpuState.flags.z = (result == 0);
    cpuState.flags.n = (result >> 63) & 1;
    cpuState.flags.c = (carry_out & 1);
}

/* ALU Ops */

static inline uint64_t doArithAdd(uint64_t a, uint64_t b) {
    uint64_t result = a + b;
    setFlagsAddSub(a, b, result, 0);
    return result;
}

static inline uint64_t doArithSub(uint64_t a, uint64_t b) {
    uint64_t result = a - b;
    setFlagsAddSub(a, b, result, 1);
    return result;
}

/* rd = rs1 + rs2 + c */
static inline uint64_t doArithAddc(uint64_t a, uint64_t b) {
    uint64_t c_in = cpuState.flags.c;
    uint64_t result = a + b + c_in;
    setFlagsAddSub(a, b + c_in, result, 0);
    return result;
}

/* rd = rs1 - rs2 - (1 - c)   [ARM/m68k convention] */
static inline uint64_t doArithSubb(uint64_t a, uint64_t b) {
    uint64_t c_in = cpuState.flags.c;
    uint64_t effective_b = b + (1 - c_in);
    uint64_t result = a - effective_b;
    setFlagsAddSub(a, effective_b, result, 1);
    return result;
}

static inline uint64_t doArithAnd(uint64_t a, uint64_t b) {
    uint64_t result = a & b;
    setFlagsLogic(result);
    return result;
}

static inline uint64_t doArithOr(uint64_t a, uint64_t b) {
    uint64_t result = a | b;
    setFlagsLogic(result);
    return result;
}

static inline uint64_t doArithXor(uint64_t a, uint64_t b) {
    uint64_t result = a ^ b;
    setFlagsLogic(result);
    return result;
}

/* shift amount masked to 6 bits */
static inline uint64_t doArithSll(uint64_t a, uint64_t b) {
    uint64_t shamt = b & 0x3F;
    uint64_t carry_out = (shamt == 0) ? 0 : (a >> (64 - shamt)) & 1;
    uint64_t result = a << shamt;
    setFlagsShift(result, carry_out);
    return result;
}

static inline uint64_t doArithSrl(uint64_t a, uint64_t b) {
    uint64_t shamt = b & 0x3F;
    uint64_t carry_out = (shamt == 0) ? 0 : (a >> (shamt - 1)) & 1;
    uint64_t result = a >> shamt;
    setFlagsShift(result, carry_out);
    return result;
}

static inline uint64_t doArithSra(uint64_t a, uint64_t b) {
    uint64_t shamt = b & 0x3F;
    uint64_t carry_out = (shamt == 0) ? 0 : (a >> (shamt - 1)) & 1;
    int64_t signed_a = (int64_t)a;
    uint64_t result = (uint64_t)(signed_a >> shamt); /* arithmetic shift on signed type */
    setFlagsShift(result, carry_out);
    return result;
}

static inline uint64_t doArithRor(uint64_t a, uint64_t b) {
    uint64_t shamt = b & 0x3F;
    uint64_t result = (shamt == 0) ? a : ((a >> shamt) | (a << (64 - shamt)));
    uint64_t carry_out = (shamt == 0) ? 0 : (a >> (shamt - 1)) & 1;
    setFlagsShift(result, carry_out);
    return result;
}

static inline uint64_t doArithRol(uint64_t a, uint64_t b) {
    uint64_t shamt = b & 0x3F;
    uint64_t result = (shamt == 0) ? a : ((a << shamt) | (a >> (64 - shamt)));
    uint64_t carry_out = (shamt == 0) ? 0 : (a >> (64 - shamt)) & 1;
    setFlagsShift(result, carry_out);
    return result;
}

static inline void doArithCmp(uint64_t a, uint64_t b) {
    uint64_t result = a - b;
    setFlagsAddSub(a, b, result, 1);
    /* result discarded */
}