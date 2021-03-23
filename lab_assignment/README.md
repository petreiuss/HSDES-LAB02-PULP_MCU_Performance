# Lab Assignment 

### Problem Description
Given a dataset of N=1M _(x,y)_ vectors stored in the external L3 memory, where _x_ and _y_ are 16-bit integers (_short int_) values, design and profile a C function 
that returns the index of the (_x,y_) vector that minimizes the L2 norm computed with respect to a given (_xm,ym_) vector. Given the high size, the dataset should be decomposed 
in small chuncks of data, each one of size _Nb_. Every chunck of the dataset need to be copied into the local L2 memory and analyzed. The analysis consists of computing
the minimum L2 distance 
```
L2_dist = min ((x[i]-xm)^2 + (y[i]-ym)^2), i=0,...Nb
```
Eventually, the function should return the minimal L2 distance and the index of the element of the dataset closer to the given (_xm,ym_) vector.

### Contraint
* The maximum L2 buffers (also global variables!!) that can be allocated corresponds to 256kB.


### Hints
* The dataset should be allocated and initialized with deterministic rules. For instance you may refer to the following pseudo-code:
```
for i=0:1M
  x[i] = i % 2^15
  y[i] = 
```
note that % 2^15 constraints the value of the vector coordinates to be within the range of an _short int_ variable.

* The initialization function should not be included within the measurement of the execution time.
* A possible (but not the only one!!) prototype of the computing function to be designed may appear as the following one:
```
int compute_L2_distance (short int * vm, short int * v, int Nb, int * L2_min){
    ....
    return idx;
}
```
* Use double buffering to minimize the execution time of the function.

### Questions
* What is the execution time in terms of clock cycles to execute the task?
* What is the optimal size of the L2 buffers to be allocated?
