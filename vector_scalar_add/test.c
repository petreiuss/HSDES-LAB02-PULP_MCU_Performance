/* 
 * Copyright (C) 2020 University of Bologna
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: 
 *  Manuele Rusci, UniBO (manuele.rusci@unibo.it),
 *  Nazareno Brusci, UniBO (nazareno.bruschi@unibo.it)        
 */

#include "pmsis.h"

//#define PRINT_VECTOR // uncomment if you want to print matrix values
#define N 1024

// global variables
int a;
int B[N];
int C[N];

// matrix functions
void task_initMat(int * array, int dim)
{
    for(int i=0;i<dim;i++){
        array[i] = i;
    }
}

void task_VectScalarAdd(int scalarA, int* vecB, int * vecC, int dim)
{
    for(int i=0; i<dim; i++){
        vecC[i] = scalarA + vecB[i];
    }
}

void print_vector(int * vec, int dim)
{
    for(int i=0;i<dim;i++){
      printf("%02d ", vec[i]);
    }
    printf("\n");

}

int main()
{
    // initialization
    a = 5; // initialize scalar value
    task_initMat(B, N); // initialize vector operand

#ifdef PRINT_VECTOR
    printf("\n\nThis is the Matrix B\n");
    print_vector(B, N);
#endif

    /* 
        Initialize performance counters.
        Available performance counters for configurations:
            (<PULP_SDK_HOME>/rtos/pmsis/pmsis_api/include/pmsis/chips/default.h)
          PI_PERF_ACTIVE_CYCLES
          PI_PERF_INSTR        
          PI_PERF_LD_STALL     
          PI_PERF_JR_STALL     
          PI_PERF_IMISS        
          PI_PERF_LD           
          PI_PERF_ST           
          PI_PERF_JUMP         
          PI_PERF_BRANCH       
          PI_PERF_BTAKEN       
          PI_PERF_RVC          
          PI_PERF_LD_EXT       
          PI_PERF_ST_EXT       
          PI_PERF_LD_EXT_CYC   
          PI_PERF_ST_EXT_CYC   
          PI_PERF_TCDM_CONT    
    */

    pi_perf_conf(
        1 << PI_PERF_CYCLES | 
        1 << PI_PERF_INSTR   
    );

    // measure statistics on matrix operations
    pi_perf_reset();
    pi_perf_start();

    // task to profile
    task_VectScalarAdd(a, B, C, N);


    pi_perf_stop(); // stop the performance counters


    // collect an print statistics
    uint32_t instr_cnt = pi_perf_read(PI_PERF_INSTR);
    uint32_t cycles_cnt = pi_perf_read(PI_PERF_CYCLES);
    float cpi = (float) cycles_cnt / instr_cnt;

    printf("Number of Instructions: %d\nClock Cycles: %d\nCPI: %f\n", 
        instr_cnt, cycles_cnt, cpi);

    //compute the checksum
    int checksum = 0;
    for(int i=0;i<N;i++){
      checksum += C[i];
    }
    printf("Checksum: %d\n", checksum);

    // return 0 
    pmsis_exit(0);
}
