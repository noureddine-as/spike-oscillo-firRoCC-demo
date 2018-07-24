#ifndef FIR_FX_H_
#define FIR_FX_H_

#define STR1(x) #x
#ifndef STR
#define STR(x) STR1(x)
#endif
#define EXTRACT(a, size, offset) (((~(~0 << size) << offset) & a) >> offset)

// rd = rs2[offset + size - 1 : offset]
// rs1 is clobbered
// rs2 is left intact
// #define EXTRACT_RAW(rd, rs1, rs2, size, offset) \
//   not x ## rs1, x0;                             \
//   slli x ## rs1, x ## rs1, size;                \
//   not x ## rs1, x ## rs1;                       \
//   slli x ## rs1, x ## rs1, offset;              \
//   and x ## rd, x ## rs1, x ## rs2;              \
//   srai x ## rd, x ## rd, offset;

#define XCUSTOM_OPCODE(x) XCUSTOM_OPCODE_ ## x
#define XCUSTOM_OPCODE_0 0b0001011
#define XCUSTOM_OPCODE_1 0b0101011
#define XCUSTOM_OPCODE_2 0b1011011
#define XCUSTOM_OPCODE_3 0b1111011

#define FIR_OPCODE      XCUSTOM_OPCODE_3

#define FIR_FX(rd, rs1, rs2, funct)         \
  FIR_OPCODE                         |         \
  (rd                   << (7))       |         \
  (0x3                  << (7+5))     |         \
  ((rd != 0) & 1        << (7+5+2))   |         \
  (rs1                  << (7+5+3))   |         \
  (rs2                  << (7+5+3+5)) |         \
  (EXTRACT(funct, 7, 0) << (7+5+3+5+5))

// #define XCUSTOM(x, rd, rs1, rs2, funct)         \
//   XCUSTOM_OPCODE(x)                   |         \
//   (rd                   << (7))       |         \
//   (0x7                  << (7+5))     |         \
//   ((rd != 0) & 1        << (7+5+2))   |         \
//   (rs1                  << (7+5+3))   |         \
//   (rs2                  << (7+5+3+5)) |         \
//   (EXTRACT(funct, 7, 0) << (7+5+3+5+5))

// #define FIR_FX_INSTRUCTION_RAW_R_R_R(rd, rs1, rs2, funct)      \
//   .word FIR_FX(## rd, ## rs1, ## rs2, funct)

// #define ROCC_INSTRUCTION_RAW_R_R_R(x, rd, rs1, rs2, funct)      \
//   .word XCUSTOM(x, ## rd, ## rs1, ## rs2, funct)

// Standard macro that passes rd, rs1, and rs2 via registers
// #define ROCC_INSTRUCTION(x, rd, rs1, rs2, funct)                \
//   ROCC_INSTRUCTION_R_R_R(x, rd, rs1, rs2, funct, 10, 11, 12)

// Standard macro that passes rd, rs1, and rs2 via registers
// Where rd = t2 = x7    rs1 = t0 = x5    rs2 = t1 = x6
// #define FIR_FX_INSTRUCTION(x, rd, rs1, rs2, funct)                \
//   FIR_FX_INSTRUCTION_R_R_R(x, rd, rs1, rs2, funct, 0, 5, 6)



// rd, rs1, and rs2 are data
// rd_n, rs_1, and rs2_n are the register numbers to use
// #define FIR_FX_INSTRUCTION_R_R_R(x, rd, rs1, rs2, funct, rd_n, rs1_n, rs2_n) \
//   {                                                                     \
//     register unsigned long int rd_  asm ("x" # rd_n);                            \
//     register unsigned long int rs1_ asm ("x" # rs1_n) = (unsigned long int) rs1;          \
//     register unsigned long int rs2_ asm ("x" # rs2_n) = (unsigned long int) rs2;          \
//     asm volatile (                                                      \
//         ".word " STR(FIR_FX(x, rd_n, rs1_n, rs2_n, funct)) "\n\t"      \
//         : "=r" (rd_)                                                    \
//         : [_rs1] "r" (rs1_), [_rs2] "r" (rs2_));                        \
//     rd = rd_;                                                           \
//   }

#define FIR_FX_INSTRUCTION_X_R_R(rs1, rs2, funct, rs1_n, rs2_n)  \
  {                                                               \
    register unsigned long int rs1_ asm ("x" # rs1_n) = (unsigned long int) rs1;    \
    register unsigned long int rs2_ asm ("x" # rs2_n) = (unsigned long int) rs2;    \
    asm volatile (                                                                  \
        ".word " STR(FIR_FX(0, rs1_n, rs2_n, funct)) "\n\t"                        \
        :: [_rs1] "r" (rs1_), [_rs2] "r" (rs2_)                                     \
         : "memory" );                                                              \
  }

#define FIR_FX_INSTRUCTION(rs1, rs2, funct)                \
  FIR_FX_INSTRUCTION_X_R_R(rs1, rs2, funct, 5, 6)

#endif  // FIR_FX_H_