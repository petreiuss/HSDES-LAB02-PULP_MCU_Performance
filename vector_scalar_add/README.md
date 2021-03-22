# Vectorial Scalar Add Example

This example code includes the function _task_VectScalarAdd_ that computes the sum *C = a + B*, where *B* and *C* are integer arrays of N elements and *a* is an integer constant.


## Measuring and Collecting Performance Counters
Every RISC-V core of the PULP Platform includes several performance counters to measure the performance statistics of the taks execution. 
The available performance counters can be found [here](https://github.com/pulp-platform/pulp-sdk/blob/main/rtos/pmsis/pmsis_api/include/pmsis/chips/default.h). 
The [PMISIS APIs](https://greenwaves-technologies.com/manuals/BUILD/PMSIS_API/html/group__Perf.html) are used to configure, control and read the available counters.

```
pi_perf_conf( 1 << PI_PERF_CYCLES | 1 << PI_PERF_INSTR ); // enable the perf counters of interest

pi_perf_reset(); // reset the performance counters
pi_perf_start(); // start the performance counters

foo(); // task to profile

pi_perf_stop(); // stop the performance counters

// collect an print statistics
uint32_t instr_cnt = pi_perf_read(PI_PERF_INSTR);
uint32_t cycles_cnt = pi_perf_read(PI_PERF_CYCLES);
printf("Number of Instructions: %d\nClock Cycles: %d\n", 
        instr_cnt, cycles_cnt);
```

## Getting Started
To get started with the example, simply:
~~~~~shell
cd vector_scalar_add/
make clean all run
~~~~~
* How many instructions and clock cycles are taken by the _task_VectScalarAdd_ function?
* What is it the measured CPI?
* How many of the instruction corresponds to load and stall and how many stalls are included with the count of the clock cycles?
Try to relate the instruction and number of clock cycles with the assembly code, which can be obtained by:
~~~~~shell
make dis > test.s
~~~~~

## System Traces


## Optimizing the code 


## Try on your own
