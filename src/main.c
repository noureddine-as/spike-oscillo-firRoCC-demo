/*======================================================================*/
/* TIMA LABORATORY                                                      */
/* Spike with signal and image visual presentation                      */ 
/* noureddine-as                                                        */
/* noureddine.aitsaid0@gmail.com                                        */
/*======================================================================*/

#include "encoding.h"
#include "util.h"
#include "FIR_COEFFS.h"
#include "fir_insns.h"

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
        fir_move(((uint64_t)h[i]) << (16 * 3) , i);
    }

    fir_move((uint64_t)0x0000000000000000, FIR_REG_PRECISION_REGISTER);

    // // pushing only one sample a time
    volatile uint64_t result_i, done;
    for(int i = 0; i < *pt_in_n_rows; i++)
    {       
        fir_fifo(((uint64_t)(*(pt_in_data  + i ))) << (16 * 3));
        fir_move(FIR_ENABLE_CALC , FIR_REG_STATUS_REGISTER);
        fir_store(&done, FIR_REG_STATUS_REGISTER);
        while(!(done & 0x8000000000000000)){
            fir_store(&done, FIR_REG_STATUS_REGISTER);
        }
        
        fir_store(&result_i, FIR_REG_RESULT_REGISTER);
        *(pt_out_data + i) = (uint8_t)(((result_i >> (4 * 3))) );


        //printf("Result [ %d ] = 0x%lx ---> 0x%x \n", i, result_i, (uint8_t)(((result_i >> (4 * 3))) ));
        // delay for spike
        volatile int j = 200;
        while(j--);
    }
        return 0;

}
