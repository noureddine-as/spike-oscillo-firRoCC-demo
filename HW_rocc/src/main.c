#include "encoding.h"
#include "in_signal.h"

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

#define S_MV   0
#define S_FIFO 1
#define S_STR  2
#define NB_COEFS 32

unsigned long int h[NB_COEFS] = 
{
  0x00d0000000000000,0x0150000000000000,0x01f0000000000000,0x02c0000000000000,0x03c0000000000000,0x04d0000000000000,0x0610000000000000,0x0750000000000000,0x08a0000000000000,0x09f0000000000000,0x0b30000000000000,0x0c50000000000000,0x0d40000000000000,0x0e10000000000000,0x0e90000000000000,0x0ee0000000000000,
  0x0ee0000000000000,0x0e90000000000000,0x0e10000000000000,0x0d40000000000000,0x0c50000000000000,0x0b30000000000000,0x09f0000000000000,0x08a0000000000000,0x0750000000000000,0x0610000000000000,0x04d0000000000000,0x03c0000000000000,0x02c0000000000000,0x01f0000000000000,0x0150000000000000,0x00d0000000000000 
};



int main() {
  // Funct(7 bits) | rs2(5 bits) | rs1(5 bits) | xd(1 bit) | xs1(1 bit) | xs2(1 bit) | rd(5 bits) | opcode(7 bits)
  // 0000001 01011 01010 000 01010 1111011
  // rs2 = a1
  // rs1 = a0
  unsigned long int y;
  // unsigned long int addr_regfile = 0;
  // unsigned long int data_source[4] = {0xdead, 0xbeef, 0x0bad, 0xf00d};
  // unsigned long int data_sink[4] = {0};
  unsigned long int read_h[NB_COEFS] = {0};
  unsigned long int status_read = 0;
  unsigned long int result = 0;

  /*
  for (int i = 0; i < 4; ++i)
  {
    printf("data_sink[%d] = %x\n", i, data_sink[i]);
  }
  */

  write_csr(mie, 0);
  write_csr(sie, 0);
  write_csr(mip, 0);
  write_csr(sip, 0);
  write_csr(mideleg, 0);
  write_csr(medeleg, 0);

  // enable machine interrupts
  __asm__("li      t0, 4096\n\t" // mcause = 12 to the RoCC int.
          "csrrs   zero, mie, t0\n\t"  // Machine External Interrupt Enable 
          "li      t0, 8\n\t"            
          "csrrs   zero, mstatus, t0\n\t" // Machine Status enable Machine Int. Enable
          );  

  volatile int aux=500;
  while(aux != 0) aux--;

  // write regfile(0) = 0xdead
  //       regfile(1) = 0x0000
  //ROCC_INSTRUCTION(0, y, data_source[0], data_source[1], 1)
  //(X, rd, rs1, rs2, funct)        

  // read coefs OK
  // for (int i = 0; i < NB_COEFS; ++i) printf("%x\n", read_h[i]);
  
  // for (int i = 0; i < NB_COEFS; ++i)
  //   ROCC_INSTRUCTION(0, y, &read_h[i], i, S_STR)  
  
  // for (int i = 0; i < NB_COEFS; ++i) printf("%x\n", read_h[i]);

  // fifo put OK
  // for (int i = 0; i < 64; ++i)
  //   ROCC_INSTRUCTION(0, y, (unsigned long int)i, 0, S_FIFO)

  //ROCC_INSTRUCTION(0, y, (unsigned long int)0x01, (unsigned long int)33, S_MV)


  // move coefs OK
  for (int i = 0; i < NB_COEFS; ++i)
    ROCC_INSTRUCTION(0, y, h[i], (unsigned long int)i, S_MV)

  for (int i = 0; i < 2; ++i)
  {
    // take a sample
    ROCC_INSTRUCTION(0, y, in_signal[i], 0, S_FIFO)
    
    // enable rocc fir
    ROCC_INSTRUCTION(0, y, (unsigned long int)0x01, (unsigned long int)33, S_MV)
    
    // read status register
    ROCC_INSTRUCTION(0, y, &status_read, 33, S_STR)

    // is it done?
    while(!(status_read & 0x8000000000000000)){
            printf("Trying to read done bit \n");
          ROCC_INSTRUCTION(0, y, &status_read, 33, S_STR) // ROCC_INSTRUCTION(0, y, &status_read, 33, S_STR)
                printf("status_read = %lx\n", status_read);

    }
                printf("it's done !", status_read);

    // it's done!
    ROCC_INSTRUCTION(0, y, &result, 34, S_STR)
    printf("result = 0x%lx\n", result);

    // acknowlegemnt that's done!
    ROCC_INSTRUCTION(0, y, (unsigned long int)0, (unsigned long int)33, S_MV)    
  }
  
  // This cause an interrupt
  // mcause = 0x800000000000000C (reserved)
  //ROCC_INSTRUCTION(0, y, &data_sink[0], 0, 3)

  /*
  ROCC_INSTRUCTION(3, y, &data_sink[1], 1, 3)
    
  ROCC_INSTRUCTION(3, y, data_source[2], data_source[3], 1)
  ROCC_INSTRUCTION(3, y, &data_sink[2], 0, 3)
  ROCC_INSTRUCTION(3, y, &data_sink[3], 1, 3)  
    
  printf("\n");
  for (int i = 0; i < 4; ++i)
  {
    printf("data_sink[%d] = %s\n", i, data_sink[i]);
  }
  */

  /* What does the RoCC? 
      He write in an address (0x80000000 - 0x9000000) 
            io.mem.req.bits.phys := Bool(false) 
      So, it makes a request to Cache with the virtual address
      and the cache knows where is the physical address of data 
      so send or modify itself */
  
  return 0;  
}