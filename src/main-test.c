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

#define FIR_REG_STATUS_REGISTER 33
#define FIR_REG_RESULT_REGISTER 34

#define FIR_ENABLE_CALC 0x01
#define FIR_ENABLE_INT  0x02
#define FIR_DONE_BIT    (0x01 << 31)



int main()
{
    init_csrs();

    // Initiating addresses
    uint8_t *base = (uint8_t *)(OSCILLO_BASE);
    uint8_t *pt_app_status = base;
    uint8_t *pt_spike_status = base+1;

    uint16_t *pt_in_n_rows = (uint16_t *)(OSCILLO_BASE +2);
    uint8_t *pt_in_data = base+4;

    uint16_t *pt_out_n_rows = (uint16_t *)(base + 4 + *(pt_in_n_rows));
    uint16_t *pt_out_data = (uint16_t *)(base + 4 + *(pt_in_n_rows) + 2) ;

    // Filling out the Coeffs reg_file of RoCC
    for(int i=0; i<N; i++)
    {
        FIR_FX_INSTRUCTION(h[i], i, F_MOVE);
        //printf("FX = %d   --  moved %d  to reg_file[%d]  --   done \n", F_MOVE, rs1, rs2);
    }

    // Filling out the FIFO of FIR RoCC with the input signal sample this is done once
    // to fill the first 32 samples. After we will begin pushing only one sample a time
    for(int i=0; i<N; i++)
    {
        FIR_FX_INSTRUCTION(*(pt_in_data + i), 0, F_FIFO);
        //printf("FX = %d   --  moved %d  to reg_file[%d]  --   done \n", F_MOVE, rs1, rs2);
    }  


    // Testing STORE ins. We'll try to store the value of an internal register in a
    // variable
    uint64_t content;
    uint64_t reg_to_store = 5;
    FIR_FX_INSTRUCTION(&content, reg_to_store, F_STORE);
    printf("\nContent of register %d  is   0x%x \n", reg_to_store, content);


    FIR_FX_INSTRUCTION(FIR_ENABLE_CALC, FIR_REG_STATUS_REGISTER, F_MOVE);

    FIR_FX_INSTRUCTION(&content, FIR_REG_RESULT_REGISTER, F_STORE);
    printf("\nContent of RESULT_REGISTER %d  is   0x%x \n", reg_to_store, content);



    /* TESTING DIFFERENT STATUS REGISTER VALUES
    */

    // This enables the INTerrupt of the FIR !
    // Meaning that if the interrupt has been enabled, once done if the calc is done
    // an interrupt should be launched !
       // FIR_FX_INSTRUCTION(FIR_ENABLE_INT, FIR_REG_STATUS_REGISTER, F_MOVE);

    // Writing to calc executes the FIR operation and turns the done bit ON and disables ENABLE bit !
       // FIR_FX_INSTRUCTION(FIR_ENABLE_CALC, FIR_REG_STATUS_REGISTER, F_MOVE);

    // Writing to Done is prohibited this should provoke an illegal instruction trap!
       // FIR_FX_INSTRUCTION(FIR_DONE_BIT, FIR_REG_STATUS_REGISTER, F_MOVE);



    // #define FX     1
    // rs1 = 0xDEAD; rs2 = 0;
    // FIR_FX_INSTRUCTION(rs1, rs2, FX);
    // printf("FX = %d   --  pushed %d  to fifo  --   done \n", FX, rs1);


//  PRILIMINARY TEST
/*
    int res = 0;
    int fifty = 50;

    #define FX     0
    res = 0;
    FIR_FX_INSTRUCTION(&res, fifty, FX);
    printf("FX = %d   --   res = %d \n", FX, res);

    #define FX     1
    res = 0;
    FIR_FX_INSTRUCTION(&res, fifty, FX);
    printf("FX = %d   --   res = %d \n", FX, res);

    #define FX     2
    res = 0;
    FIR_FX_INSTRUCTION(&res, fifty, FX);
    printf("FX = %d   --   res = %d \n", FX, res);  

    #define FX     3
    res = 0;
    FIR_FX_INSTRUCTION(&res, fifty, FX);
    printf("FX = %d   --   res = %d \n", FX, res);

    #define FX     4
    res = 0;
    FIR_FX_INSTRUCTION(&res, fifty, FX);
    printf("FX = %d   --   res = %d \n", FX, res);  

    #define FX     5
    res = 0;
    FIR_FX_INSTRUCTION(&res, fifty, FX);
    printf("FX = %d   --   res = %d \n", FX, res);

    #define FX     6
    res = 0;
    FIR_FX_INSTRUCTION(&res, fifty, FX);
    printf("FX = %d   --   res = %d \n", FX, res);  

    #define FX     7
    res = 0;
    FIR_FX_INSTRUCTION(&res, 50, FX);
    printf("FX = %d   --   res = %d \n", FX, res);  
*/
    return 0;
}

/*

#define OSCILLO_BASE 0x82000000

int x_index = 0, i=0;
uint8_t Xn;
uint32_t Yn;
uint8_t XnBuffer[N+1];

int main(int argc, char** argv)
{
    init_csrs();
    for ( i = 0; i < N+1; i++){
        XnBuffer[i] = 0;
    }

    uint8_t *base = (uint8_t *)(OSCILLO_BASE);
    uint8_t *pt_app_status = base;
    uint8_t *pt_spike_status = base+1;

    uint16_t *pt_in_n_rows = (uint16_t *)(OSCILLO_BASE +2);
    uint8_t *pt_in_data = base+4;

    uint16_t *pt_out_n_rows = (uint16_t *)(base + 4 + *(pt_in_n_rows));
    uint16_t *pt_out_data = (uint16_t *)(base + 4 + *(pt_in_n_rows) + 2) ;

    // Deactivate Output for the moment
    *pt_app_status = (uint8_t)0x03;

    printf("[BAREMETAL - INFO] ------------------------------------\n"
           "[BAREMETAL - INFO] >> At 0x%x = 0x%lx \n", base, *base);
    printf("[BAREMETAL - INFO] app_status   = 0x%x \n", *pt_app_status);
    printf("[BAREMETAL - INFO] spike_status = 0x%x \n", *pt_spike_status);
    printf("[BAREMETAL - INFO / INPUT] ------------------------------------\n");
    printf("[BAREMETAL - INFO / INPUT] in_n_rows       = %d \n", *pt_in_n_rows);
    printf("[BAREMETAL - INFO / INPUT] in_data[0]      = 0x%x \n", *pt_in_data);
    printf("[BAREMETAL - INFO / OUTPUT] ------------------------------------\n");
    printf("[BAREMETAL - INFO / OUTPUT] out_n_rows       = %d \n", *pt_out_n_rows);
    printf("[BAREMETAL - INFO / OUTPUT] out_data[0]      = 0x%x \n", *pt_out_data);

    volatile int i, j;
    for(i = 0; i< *(pt_in_n_rows); i++)
    {
        Yn = 0;
        XnBuffer[0] = *(pt_in_data + i);
        uint64_t t_0 = rdcycle();

        //printf("input[ %d ] = 0x%x \n", i, *(pt_in_data + i));
        for ( j = N-1; j >=0 ; j-- ){
            Yn = (uint32_t)(Yn + ((uint32_t)h[j] * (uint32_t)XnBuffer[j]));
            XnBuffer[j+1] = XnBuffer[j];
            // N+1 -> N   poubelle
        }

        t_0 = rdcycle() - t_0;
        //printf("cycles [ %d ] = %d \n", i, t_0);

        // Output on 8 bits
        *(pt_out_data + i) = (Yn >> 12);
    }


    printf("Waiting ... \n");
    volatile int k = 1000;
    while(k--);

    return 0;
}
*/