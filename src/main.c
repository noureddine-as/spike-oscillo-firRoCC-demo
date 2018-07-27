/*======================================================================*/
/* TIMA LABORATORY                                                      */
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
    FIR_FX_INSTRUCTION(value, reg_file, F_MOVE);    
}

void fir_fifo(uint64_t value)
{
    FIR_FX_INSTRUCTION(value, 0, F_FIFO);
}

void fir_store(uint64_t* addr, uint64_t reg_file)
{
    FIR_FX_INSTRUCTION(addr, reg_file, F_STORE);
}

int main()
{
    unsigned long int y;
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
    // *pt_app_status = (uint8_t)0x03;

    // // Filling out the Coeffs reg_file of RoCC
    for(int i=0; i<N; i++)
    {
        //FIR_FX_INSTRUCTION(h[i], i, F_MOVE);
        fir_move(h[i] , i);
    }

    fir_move((uint64_t)0x0000000000000001, FIR_REG_PRECISION_REGISTER);


    // pushing only one sample a time
    uint64_t result_i, done;
    volatile int j;
    for(int i = 0; i < *pt_in_n_rows; i++)
    {
        //FIR_FX_INSTRUCTION(*(pt_in_data + i), 0, F_FIFO);
        fir_fifo(*(pt_in_data + i));

        //FIR_FX_INSTRUCTION(FIR_ENABLE_CALC, FIR_REG_STATUS_REGISTER, F_MOVE);
        fir_move(FIR_ENABLE_CALC , FIR_REG_STATUS_REGISTER);

        //FIR_FX_INSTRUCTION(&done, FIR_REG_STATUS_REGISTER, F_STORE);
        fir_store(&done, FIR_REG_STATUS_REGISTER);

        done = 0;
        while(!(done & 0x8000000000000000)){
            fir_store(&done, FIR_REG_STATUS_REGISTER);

            //FIR_FX_INSTRUCTION(&done, FIR_REG_STATUS_REGISTER, F_STORE);
            //printf("******** FIR operation NOT YET done. \n");
        }

        //FIR_FX_INSTRUCTION(&result_i, FIR_REG_RESULT_REGISTER, F_STORE);
        fir_store(&result_i, FIR_REG_RESULT_REGISTER);
    
        *(pt_out_data + i) = (uint8_t)((result_i >> 12) & 0xFF);

        volatile int j = 200;
        while(j--);

    //    printf("\n*** FIR operation COMPLETED -- Result[ %d ] = 0x%x \n", i, result_i);

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
