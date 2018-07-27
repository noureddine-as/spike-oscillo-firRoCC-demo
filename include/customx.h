#include "encoding.h"

#define STR1(x) #x
#define STR(x) STR1(x)
#define EXTRACT(a, size, offset) (((~(~0 << size) << offset) & a) >> offset)

#define CUSTOMX_OPCODE(x) CUSTOM_ ## x
#define CUSTOM_0 0b0001011
#define CUSTOM_1 0b0101011
#define CUSTOM_2 0b1011011
#define CUSTOM_3 0b1111011

// xd xs1 xs2 using always
#define CUSTOMX(X, rd, rs1, rs2, funct)         \
  CUSTOMX_OPCODE(X)                   |         \
  (rd                   << (7))       |         \
  (0x3                  << (7+5))     |         \
  (rs1                  << (7+5+3))   |         \
  (rs2                  << (7+5+3+5)) |         \
  (EXTRACT(funct, 7, 0) << (7+5+3+5+5))

// 0b011 = 0x3 -> if we enable xd we have an exception

// Standard macro that passes rd, rs1, and rs2 via registers
#define ROCC_INSTRUCTION(X, rd, rs1, rs2, funct)                \
  ROCC_INSTRUCTION_R_R_R(X, rd, rs1, rs2, funct, 5, 6, 7)

// ROCC_INSTRUCTION_R_R_R(X, rd, rs1, rs2, funct, 10, 11, 12)

// rd, rs1, and rs2 are data
// rd_n, rs_1, and rs2_n are the register numbers to use
#define ROCC_INSTRUCTION_R_R_R(X, rd, rs1, rs2, funct, rd_n, rs1_n, rs2_n) {              \
    register unsigned long int rd_  asm ("x" # rd_n);                                     \
    register unsigned long int rs1_ asm ("x" # rs1_n) = (unsigned long int) rs1;          \
    register unsigned long int rs2_ asm ("x" # rs2_n) = (unsigned long int) rs2;          \
    asm volatile (                                                      \
        ".word " STR(CUSTOMX(X, rd_n, rs1_n, rs2_n, funct)) "\n\t"      \
        : "=r" (rd_)                                                    \
        : [_rs1] "r" (rs1_), [_rs2] "r" (rs2_)                          \
        : "cc"                                                          \
        );                                                              \
    rd = rd_;                                                           \
  }


#define k_DO_WRITE 0
#define XCUSTOM_ACC 0
#define doWrite(y, rocc_rd, data)                                       \
  ROCC_INSTRUCTION(XCUSTOM_ACC, y, data, rocc_rd, k_DO_WRITE);

