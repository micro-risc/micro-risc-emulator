#pragma once

#define FUNCT4_START 12
#define FUNCT4_END 15

#define RD_START 8
#define RD_END 11

#define RS1_START 16
#define RS1_END 19

#define RS2_START 20
#define RS2_END 23

#define FUNCT8_START 24
#define FUNCT8_END 31

#define I_TYPE_IMM_START 20
#define I_TYPE_IMM_END 31

#define B_TYPE_IMM_START 16
#define B_TYPE_IMM_END 31

#define LI_TYPE_IMM_START 12
#define LI_TYPE_IMM_END 31

#define J_TYPE_IMM_START 8
#define J_TYPE_IMM_END 31

#define EXTRACT_R_TYPE(i) 	uint8_t funct4 	= extractBitfieldU((uint64_t)(i), FUNCT4_END, FUNCT4_START); \
							uint8_t rs1 	= extractBitfieldU((uint64_t)(i), RS1_END, RS1_START); \
							uint8_t rs2 	= extractBitfieldU((uint64_t)(i), RS2_END, RS2_START); \
							uint8_t rd 		= extractBitfieldU((uint64_t)(i), RD_END, RD_START); \
							uint64_t reg1 	= cpuState.registers[rs1]; \
							uint64_t reg2 	= cpuState.registers[rs2]

#define EXTRACT_I_TYPE(i) 	uint8_t funct4 	= extractBitfieldU((uint64_t)(i), FUNCT4_END, FUNCT4_START); \
							uint8_t rs1 	= extractBitfieldU((uint64_t)(i), RS1_END, RS1_START); \
							uint8_t rd 		= extractBitfieldU((uint64_t)(i), RD_END, RD_START); \
							uint64_t reg1 	= cpuState.registers[rs1]; \
							uint64_t imm 	= extractBitfieldS((uint64_t)(i), I_TYPE_IMM_END, I_TYPE_IMM_START)

#define EXTRACT_B_TYPE(i) 	uint8_t funct4 	= extractBitfieldU((uint64_t)(i), FUNCT4_END, FUNCT4_START); \
							uint64_t imm 	= ( extractBitfieldS((uint64_t)(i), B_TYPE_IMM_END, B_TYPE_IMM_START) << 6 ) | ( extractBitfieldU((uint64_t)(i), RD_END, RD_START) << 2 )

#define EXTRACT_LI_TYPE(i) 	uint8_t rd 		= extractBitfieldU((uint64_t)(i), RD_END, RD_START); \
							uint64_t imm 	= extractBitfieldS((uint64_t)(i), LI_TYPE_IMM_END, LI_TYPE_IMM_START)

#define EXTRACT_J_TYPE(i) 	uint64_t imm 	= ( extractBitfieldS((uint64_t)(i), J_TYPE_IMM_END, J_TYPE_IMM_START) << 2)


#define EXTRACT_S_TYPE(i) 	uint8_t funct4 	= extractBitfieldU((uint64_t)(i), FUNCT4_END, FUNCT4_START); \
							uint8_t rs1 	= extractBitfieldU((uint64_t)(i), RS1_END, RS1_START); \
							uint8_t rs2 	= extractBitfieldU((uint64_t)(i), RS2_END, RS2_START); \
							uint64_t reg2 	= cpuState.registers[rs2]; \
							uint64_t reg1 	= cpuState.registers[rs1]; \
							uint64_t imm 	= ( extractBitfieldS((uint64_t)(i), FUNCT8_END, FUNCT8_START) << 4 ) | ( extractBitfieldU((uint64_t)(i), RD_END, RD_START) )
