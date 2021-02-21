/* 
 * Copyright (C) 2020 University of Bologna
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Manuele Rusci, UniBO (manuele.rusci@unibo.it)
 */

#include "pmsis.h"

//#define PRINT_MATRIX 
#define N 10

// global variables
int A[N*N];
int B[N*N];
int C[N*N];
// Q: where are the array allocated?


// matrix functions
void task_initMat()
{
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            A[i*N+j] = i;
            B[i*N+j] = j;
        }
    }
}

void task_MatAdd(int * matA, int* matB, int * matC, int dim)
{
    for(int i=0;i<dim*dim;i++){
        matC[i] = matA[i] + matB[i];
    }
}

void print_matrix(int * mat, int dim)
{
    for(int i=0;i<dim;i++){
        for(int j=0;j<dim;j++){
            printf("%02d ", mat[i*dim+j]);
        }
        printf("\n");
    }
}

int main()
{

    //initialize performance counters
    pi_perf_conf(
        1 << PI_PERF_CYCLES | 
        1 << PI_PERF_INSTR 
    );

    // initialize matrix operands
    task_initMat();

#ifdef PRINT_MATRIX
    printf("\n\nThis is the Matrix A\n");
    print_matrix(A, N);
    printf("\n\nThis is the Matrix B\n");
    print_matrix(B, N);
#endif

    // measure statistics on matrix operations
    pi_perf_reset();
    pi_perf_start();

    task_MatAdd(A, B, C, N);

    pi_perf_stop();
    uint32_t instr_cnt = pi_perf_read(PI_PERF_INSTR);
    uint32_t cycles_cnt = pi_perf_read(PI_PERF_CYCLES);

    printf("Number of Instructions: %d\nClock Cycles: %d\nCPI: %f%f\n", 
        instr_cnt, cycles_cnt, (float) cycles_cnt/instr_cnt);
    // Q: is the number of clock cycles reasonable?


#ifdef PRINT_MATRIX
    printf("\n\nThe result of the MatAdd is C\n");
    print_matrix(C, N);
#endif

}