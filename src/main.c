/*======================================================================*/
/* TIMA LABORATORY                                                      */
/*======================================================================*/
#include "encoding.h"
#include "util.h"
#include "FIR_COEFFS.h"
#include "fir_insns.h"
//#include "in_signal.h"

void init_csrs()
{
    write_csr(mie, 0);
    write_csr(sie, 0);
    write_csr(mip, 0);
    write_csr(sip, 0);
    write_csr(mideleg, 0);
    write_csr(medeleg, 0);

    __asm__("li      t0, 4096\n\t" // mcause = 12 to the RoCC int.
          "csrrs   zero, mie, t0\n\t"  // Machine External Interrupt Enable 
          "li      t0, 8\n\t"            
          "csrrs   zero, mstatus, t0\n\t" // Machine Status enable Machine Int. Enable
          );  

}


#define OSCILLO_BASE 0x82000000

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

void fir_store(uint64_t* addr, uint64_t reg_file)
{
    //FIR_FX_INSTRUCTION(addr, reg_file, F_STORE);
    uint64_t y;
    ROCC_INSTRUCTION(0, y, addr, reg_file, F_STORE);  
}

// #define N 32
// unsigned long int h[N] = 
// {
//   0x00d0000000000000,0x0150000000000000,0x01f0000000000000,0x02c0000000000000,0x03c0000000000000,0x04d0000000000000,0x0610000000000000,0x0750000000000000,0x08a0000000000000,0x09f0000000000000,0x0b30000000000000,0x0c50000000000000,0x0d40000000000000,0x0e10000000000000,0x0e90000000000000,0x0ee0000000000000,
//   0x0ee0000000000000,0x0e90000000000000,0x0e10000000000000,0x0d40000000000000,0x0c50000000000000,0x0b30000000000000,0x09f0000000000000,0x08a0000000000000,0x0750000000000000,0x0610000000000000,0x04d0000000000000,0x03c0000000000000,0x02c0000000000000,0x01f0000000000000,0x0150000000000000,0x00d0000000000000 
// };


int main()
{
    init_csrs();

    // // Initiating addresses
    uint8_t *base = (uint8_t *)(OSCILLO_BASE);
    uint8_t *pt_app_status = base;
    uint8_t *pt_spike_status = base+1;

    uint16_t *pt_in_n_rows = (uint16_t *)(OSCILLO_BASE + 2);
    uint8_t *pt_in_data = (uint8_t *)(OSCILLO_BASE + 4);

    uint16_t *pt_out_n_rows = (uint16_t *)(base + 4 + *(pt_in_n_rows));
    uint16_t *pt_out_data = (uint16_t *)(base + 4 + *(pt_in_n_rows) + 2) ;

    // // Activate Output + Input for the moment
    *pt_app_status = (uint8_t)0x03;

    // // Filling out the Coeffs reg_file of RoCC
    for(int i=0; i<N; i++)
    {
        //FIR_FX_INSTRUCTION(h[i], i, F_MOVE);

        printf("H[ %d ] = 0x%lx \n", i, ((uint64_t)h[i]) << (16 * 3));
        fir_move(((uint64_t)h[i]) << (16 * 3) , i); //*/, (uint64_t)i);
        // fir_move(h[i] , i); //*/, (uint64_t)i);
    }
        

    fir_move((uint64_t)0x0000000000000000, FIR_REG_PRECISION_REGISTER);

uint64_t y;
    // // pushing only one sample a time
    volatile uint64_t result_i, done;
    volatile int j;
    //for(int i = 0; i <  *pt_in_n_rows; i++)
    for(int i = 0; i < *pt_in_n_rows; i++)
    {
        
        //fir_fifo(in_signal[i]); // */);
        
        fir_fifo(((uint64_t)(*(pt_in_data  + i ))) << (16 * 3)); // */);
        //printf("X[ %d ] = 0x%lx \n", i, ((uint64_t)h[i]) << (16 * 3));

        //fir_fifo((uint64_t)(*(pt_in_data  + i ))); // */);

        fir_move(FIR_ENABLE_CALC , FIR_REG_STATUS_REGISTER);

        fir_store(&done, FIR_REG_STATUS_REGISTER);
        while(!(done & 0x8000000000000000)){
            //printf("Trying to read %lx \n", done);
            //ROCC_INSTRUCTION(0, y, &done, 33, F_STORE) // ROCC_INSTRUCTION(0, y, &status_read, 33, S_STR)
            fir_store(&done, FIR_REG_STATUS_REGISTER);
            //printf("done = %lx\n", done);
            //printf("******** FIR operation NOT YET done. \n");
        }
       // printf("that was done !\n", done);
    // ROCC_INSTRUCTION(0, y, &result_i, 34, F_STORE)

        fir_store(&result_i, FIR_REG_RESULT_REGISTER);
      //printf("Result [ %d ] = %lx ---> %lx\n", i, result_i, (uint8_t)((result_i >> (13 * 4))));
     // printf("Result [ %d ] = 0x%lx   ---->> décalée = 0x%lx \n", i, result_i, (uint8_t)(((result_i >> (4 * 3))) ));

        *(pt_out_data + i) = (uint8_t)(((result_i >> (4 * 3))) );

        volatile int j = 200;
        while(j--);

    // //    printf("\n*** FIR operation COMPLETED -- Result[ %d ] = 0x%x \n", i, result_i);

    }

    // TESTING DIFFERENT STATUS REGISTER VALUES
    

    // This enables the INTerrupt of the FIR !
    // Meaning that if the interrupt has been enabled, once done if the calc is done
    // an interrupt should be launched !
       // FIR_FX_INSTRUCTION(FIR_ENABLE_INT, FIR_REG_STATUS_REGISTER, F_MOVE);

    // Writing to calc executes the FIR operation and turns the done bit ON and disables ENABLE bit !
       // FIR_FX_INSTRUCTION(FIR_ENABLE_CALC, FIR_REG_STATUS_REGISTER, F_MOVE);

    // Writing to Done is prohibited this should provoke an illegal instruction trap!
       // FIR_FX_INSTRUCTION(FIR_DONE_BIT, FIR_REG_STATUS_REGISTER, F_MOVE);


    return 0;
}
