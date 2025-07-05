#pragma once
#include <common.h>

typedef enum {
  IN_NONE,
  IN_NOP,
  IN_LD,
  IN_LDH,
  IN_ADC,
  IN_ADD,
  IN_CP,
  IN_DEC,
  IN_INC,
  IN_SBC,
  IN_SUB,
  IN_AND,
  IN_CPL,
  IN_OR,
  IN_XOR,
  IN_BIT,
  IN_RES,
  IN_SET,
  IN_RL,
  IN_RLA,
  IN_RLC,
  IN_RLCA,
  IN_RR,
  IN_RRA,
  IN_RRC,
  IN_RRCA,
  IN_SLA,
  IN_SRA,
  IN_SRL,
  IN_SWAP,
  IN_CALL,
  IN_JP,
  IN_JR,
  IN_RET,
  IN_RETI,
  IN_RST,
  IN_CCF,
  IN_SCF,
  IN_POP,
  IN_PUSH,
  IN_DI,
  IN_EI,
  IN_HALT,
  IN_DAA,
  IN_STOP,
  IN_CB
} in_type;

typedef enum {
  CT_NONE,
  CT_NZ,
  CT_Z,
  CT_NC,
  CT_C
} cond_type;

typedef enum {
  AM_IMP,
  AM_R_D16,
  AM_R_R,
  AM_MR_R,
  AM_R,
  AM_R_D8,
  AM_R_MR,
  AM_R_HLI,
  AM_R_HLD,
  AM_HLI_R,
  AM_HLD_R,
  AM_R_A8,
  AM_A8_R,
  AM_HL_SPR,
  AM_D16,
  AM_D8,
  AM_D16_R,
  AM_MR_D8,
  AM_MR,
  AM_A16_R,
  AM_R_A16,
} addr_mode;

typedef enum {
  RT_NONE,
  RT_A,
  RT_F,
  RT_B,
  RT_C,
  RT_D,
  RT_E,
  RT_H,
  RT_L,
  RT_AF,
  RT_BC,
  RT_DE,
  RT_HL,
  RT_SP,
  RT_PC
} reg_type;

typedef struct {
  in_type type;
  addr_mode mode;
  reg_type reg_1;
  reg_type reg_2;
  cond_type cond;
  uint8_t param;
} instruction;

instruction* instruction_by_opcode(uint8_t opcode); 

const char* inst_name(in_type t);