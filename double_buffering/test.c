/* 
 * Copyright (C) 2020 University of Bologna
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: 
 *      Nazareno Bruschi, UniBO (nazareno.bruschi@unibo.it)
 *      Manuele Rusci, UniBO (manuele.rusci@unibo.it)
 */

#include "pmsis.h"
#include <bsp/bsp.h>    // including bps for interfacing with the RAM external devices

//#define PRINT_MATRIX
#define CHECK_RESULTS
#define N               (512)
#define N_BYTE          (N*sizeof(int))

/***
     Global Variables 
***/
// operand inputs
PI_L2 int A[N];   
PI_L2 int B[N];
// global struct define
static struct pi_device ram;
static struct pi_hyperram_conf conf;
// output operand on external memory
static uint32_t C_ptr_L3; 


// additional variables for L3 buffering
#ifndef DOUBLE_BUFFERING
    int tempC[N];
#else
    int tempC[2*N];
    static struct pi_task ram_write_task;
    static int ram_returns = 0;
    // Callback for asynchronous ram write
    static void end_of_tx(void *arg)
    {
      //printf("End of %d TX \n", count);
      ram_returns++;
    }
#endif

// need for verification
#ifdef CHECK_RESULTS
    char memC[N_BYTE];
#endif

// matrix functions
void task_initVec(int * vec, int dim)
{
    for(int i=0; i<dim; i++){
        vec[i] = i;
    }
}

// micro kernel task
void task_VectProdScalar(int scalarA, int* vecB, int * vecC, int dim)
{
    for(int i=0;i<dim;i++){
        vecC[i] = scalarA * vecB[i];
    }
}

//utils
void print_vector(int * vec, int dim)
{
    for(int i=0; i<dim; i++){
        printf("%02d ", vec[i]);
    }
    printf("\n");
}

// main function
int main()
{
    printf("**** This is the Double Buffering Example! ****\n");
    // Init & open external device ram
    pi_hyperram_conf_init(&conf);
    pi_open_from_conf(&ram, &conf);
    if (pi_ram_open(&ram))
    {
        printf("Error ram open !\n");
        pmsis_exit(-3);
    }

    // Allocate output buffer
    if (pi_ram_alloc(&ram, &C_ptr_L3, N_BYTE*N_BYTE))
    {
        printf("Ram malloc failed !\n");
        pmsis_exit(-4);
    }

    // initialize input vector operands
    task_initVec(A, N);
    task_initVec(B, N);

#ifdef PRINT_MATRIX
    printf("\n\nThis is the Vector A\n");
    print_vector(A, N);
    printf("\n\nThis is the Vector B\n");
    print_vector(B, N);
#endif

    //initialize performance counters
    pi_perf_conf(
        1 << PI_PERF_CYCLES |  1 << PI_PERF_ACTIVE_CYCLES |
        1 << PI_PERF_INSTR 
    );

    // measure statistics on matrix operations
    pi_perf_reset();
    pi_perf_start();


    /* task to measure  */


#ifndef DOUBLE_BUFFERING

    // compute the output matrix as the composition of N vector of size N
    for(int i=0; i<N;i++){
        task_VectProdScalar(A[i], B, tempC, N); // the N-th output vector is stored in tempC
        // tempC is written in RAM at a given location
        // pi_ram_write is a blocking function and the FC idles until the transfer completes
        // check: https://greenwaves-technologies.com/manuals/BUILD/PMSIS_BSP/html/group__Ram.html
        pi_ram_write(&ram, C_ptr_L3+(i*N_BYTE), tempC, (uint32_t) N_BYTE);
    }

#else
    
    int i_curr=0;   // index of the current output vector row under processing
    int i_prev=0;   // index of the previous output vector row under processing

    int buffer_id = 0;  // id of the temporary buffer: can be 0 or 1

    // launche the 
    task_VectProdScalar(A[i_curr], B, &tempC[buffer_id], N);
    i_curr++;


    for(i_curr; i_curr<N; i_curr++,i_prev++){

        // compute the buffer ID as the last bit of the i_curr
        // *   buffer_id: used to store the output of the fucntion
        // * 1-buffer_id: trasffered to memory
        buffer_id = i_curr & 0x1;   
        // launch the trasnfer of the previously computed output 
        pi_ram_write_async(&ram, C_ptr_L3+(i_prev*N_BYTE), &tempC[N*(1-buffer_id)], (uint32_t) N_BYTE, pi_task_callback(&ram_write_task, end_of_tx, NULL));
        // compute the next output
        task_VectProdScalar(A[i_curr], B, &tempC[N*buffer_id], N);
        
        // wait the previous transfer to complete before launching the next one
        while(ram_returns != i_curr) {
            pi_yield(); // go to idle if not until an event (enf of write) is detected
        }
    }

    // write the last chuck of data
    pi_ram_write_async(&ram, C_ptr_L3+(N-1)*N_BYTE, &tempC[N*buffer_id], (uint32_t) N_BYTE, pi_task_callback(&ram_write_task, end_of_tx, NULL));    // last transfer

    while(ram_returns != i_curr) {
        pi_yield();
    }

#endif

    pi_perf_stop();

    // collect and print statistics
    uint32_t instr_cnt = pi_perf_read(PI_PERF_INSTR);
    uint32_t act_cycles_cnt = pi_perf_read(PI_PERF_ACTIVE_CYCLES);
    uint32_t cycles_cnt = pi_perf_read(PI_PERF_CYCLES);

    printf("Number of Instructions: %d\nClock Cycles: %d\nActive Clock Cycles: %d\nCPI: %f\n", 
        instr_cnt, cycles_cnt, act_cycles_cnt, (float) cycles_cnt/instr_cnt);

#ifdef CHECK_RESULTS
    // read back and check results
    for(int i=0; i<N;i++){
        pi_ram_read(&ram, C_ptr_L3+i*N_BYTE, memC, (uint32_t) N_BYTE);
        task_VectProdScalar(A[i], B, tempC, N);
        for(int j=0; j<N;j++){
            if(tempC[j] != *((int*) memC+j)){
                printf("Error, row: %d, index: %d, expected: 0x%x, read: 0x%x\n", i, j, tempC[j], *((int*) memC+j));
                pmsis_exit(-5);
            }
        }
    }

    printf("Test success\n");
#endif

    pi_ram_free(&ram, C_ptr_L3, N_BYTE*N_BYTE);
    pi_ram_close(&ram);

    pmsis_exit(0);
}
