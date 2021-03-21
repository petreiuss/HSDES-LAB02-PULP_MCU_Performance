/* 
 * Copyright (C) 2020 University of Bologna
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: 
 *  .......     
 */

#include "pmsis.h"

// define
//......


// global variables
//......


// add your functions here
//......


int main()
{

    // intialize variable and function
    //.......

    // collects statistics from your functions
    pi_perf_conf(
        1 << PI_PERF_CYCLES | 
        1 << PI_PERF_INSTR   
    );

    // measure statistics on matrix operations
    pi_perf_reset();
    pi_perf_start();

    // task to profile
    //.......
    //.......

    // stop the performance counters
    pi_perf_stop(); 

    // collect an print statistics
    uint32_t instr_cnt = pi_perf_read(PI_PERF_INSTR);
    uint32_t cycles_cnt = pi_perf_read(PI_PERF_CYCLES);
    float cpi = (float) cycles_cnt / instr_cnt;

    printf("Number of Instructions: %d\nClock Cycles: %d\nCPI: %f\n", 
        instr_cnt, cycles_cnt, cpi);

    // check if your function behaves correctly
    int checksum = 0;
    //.... compute and check the checksum
    printf("Checksum: %d\n", checksum);

    // return 0 
    pmsis_exit(0);
}
