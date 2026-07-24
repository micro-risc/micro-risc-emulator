#pragma once
#include <stdint.h>

/* Shared Flag Funcs */
void setFlagsAddSub(uint64_t a, uint64_t b, uint64_t result, int is_sub);

/* and/or/xor */
void setFlagsLogic(uint64_t result);

/* Shifts */
void setFlagsShift(uint64_t result, uint64_t carry_out);

/* ALU Ops */
uint64_t doArithAdd(uint64_t a, uint64_t b);
uint64_t doArithSub(uint64_t a, uint64_t b);

/* rd = rs1 + rs2 + c */
uint64_t doArithAddc(uint64_t a, uint64_t b);
/* rd = rs1 - rs2 - (1 - c)   [ARM/m68k convention] */
uint64_t doArithSubb(uint64_t a, uint64_t b);

uint64_t doArithAnd(uint64_t a, uint64_t b);
uint64_t doArithOr(uint64_t a, uint64_t b);
uint64_t doArithXor(uint64_t a, uint64_t b);

/* shift amount masked to 6 bits */
uint64_t doArithSll(uint64_t a, uint64_t b);
uint64_t doArithSrl(uint64_t a, uint64_t b);
uint64_t doArithSra(uint64_t a, uint64_t b);
uint64_t doArithRor(uint64_t a, uint64_t b);
uint64_t doArithRol(uint64_t a, uint64_t b);

void doArithCmp(uint64_t a, uint64_t b);
