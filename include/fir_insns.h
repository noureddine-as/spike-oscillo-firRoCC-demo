#ifndef FIR_FX_H_
#define FIR_FX_H_

#define STR1(x) #x
#define STR(x) STR1(x)
#define EXTRACT(a, size, offset) (((~(~0 << size) << offset) & a) >> offset)

#define CUSTOMX_OPCODE(x) CUSTOM_ ## x
#define CUSTOM_0 0b0001011
#define CUSTOM_1 0b0101011
#define CUSTOM_2 0b1011011
#define CUSTOM_3 0b1111011

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



#define FIR_OPCODE      CUSTOM_0

#define FIR_FX(rd, rs1, rs2, funct)         \
  FIR_OPCODE                         |         \
  (rd                   << (7))       |         \
  (0x3                  << (7+5))     |         \
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

#define CUSTOMX(X, rd, rs1, rs2, funct)         \
  CUSTOMX_OPCODE(X)                   |         \
  (rd                   << (7))       |         \
  (0x3                  << (7+5))     |         \
  (rs1                  << (7+5+3))   |         \
  (rs2                  << (7+5+3+5)) |         \
  (EXTRACT(funct, 7, 0) << (7+5+3+5+5))

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

#define ROCC_INSTRUCTION(X, rd, rs1, rs2, funct)                \
  ROCC_INSTRUCTION_R_R_R(X, rd, rs1, rs2, funct, 5, 6, 7)


#define F_MOVE   0
#define F_FIFO   1
#define F_STORE   2

#define FIR_REG_PRECISION_REGISTER      32
#define FIR_REG_STATUS_REGISTER         33
#define FIR_REG_RESULT_REGISTER         34

#define FIR_ENABLE_CALC 0x01
#define FIR_ENABLE_INT  0x02
#define FIR_DONE_BIT    (0x01 << 31)

void fir_move(uint64_t value, uint64_t reg_file)
{
    //FIR_FX_INSTRUCTION(value, reg_file, F_MOVE);  
    
    uint64_t y;
    ROCC_INSTRUCTION(0, y, value, reg_file, F_MOVE);  
}

void fir_fifo(uint64_t value)
{
    //FIR_FX_INSTRUCTION(value, 0, F_FIFO);

    uint64_t y;
    ROCC_INSTRUCTION(0, y, value, 0, F_FIFO);  
}

void fir_store(volatile uint64_t* addr, uint64_t reg_file)
{
    //FIR_FX_INSTRUCTION(addr, reg_file, F_STORE);
    uint64_t y;
    ROCC_INSTRUCTION(0, y, addr, reg_file, F_STORE);  
}

#endif  // FIR_FX_H_
