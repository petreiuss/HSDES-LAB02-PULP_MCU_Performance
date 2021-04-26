/* 
 * Copyright (C) 2020 University of Bologna
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: 
 *  Manuele Rusci     
 */
/
#include "pmsis.h"
#include <bsp/bsp.h>    

// defines
#define DS_SIZE  (1<<20)            // dataset size
#define Nb       (512)              // size of the dataset block
#define N_BLOCK  (DS_SIZE/Nb)       // number of blocks, i.e. chunks
#define Nb_BYTES (Nb*sizeof(int))   // size of the dataset chunk in bytes

// given vector
#define XM_USER (16)
#define YM_USER (7)

// global struct define
static struct pi_device ram;
static struct pi_hyperram_conf conf;
// output operand on external memory
static char dataset_L3; 

int temp_buffer[2*Nb];  // if Nb=512 -> Memory = 4kB

static struct pi_task ram_read_task;
static int ram_returns = 0;

// Callback for asynchronous ram write
static void end_of_read(void *arg)
{
  ram_returns++;
}

// value to search
PI_L2 short int xm = XM_USER;
PI_L2 short int ym = YM_USER;

// additional vars
short int * buffer_to_compute;
short int x, y;
int dist_x, dist_y, L2 ;


int main()
{    

    printf("**** This is the LAB02 Assignment Solution! ****\n");
    // Init & open external device ram
    pi_hyperram_conf_init(&conf);
    pi_open_from_conf(&ram, &conf);
    if (pi_ram_open(&ram))
    {
        printf("Error ram open !\n");
        pmsis_exit(-3);
    }

    // Allocate output buffer
    if (pi_ram_alloc(&ram, &dataset_L3, DS_SIZE*sizeof(int)))
    {
        printf("Ram malloc failed !\n");
        pmsis_exit(-4);
    }

    // initialize the dataset
    printf("Going to inizialize %d chuncks of %d vectors for a total of %d elements\n",
        N_BLOCK,Nb, DS_SIZE);
    int v = 0;
    for(int j = 0; j<N_BLOCK;j++){
        for(int i = 0; i<Nb; i++){
            short int x = (short int)( v % (1<<14) );
            short int y = (short int)( v % (1<<14) );
            temp_buffer[i] = (int)((x << 16 )| y);
            v++;
        }
        pi_ram_write(&ram, dataset_L3+(j*Nb_BYTES), temp_buffer, (uint32_t) Nb_BYTES);
    }


    // collects statistics from your functions
    pi_perf_conf(
        1 << PI_PERF_CYCLES | 
        1 << PI_PERF_INSTR   
    );

    // measure statistics on matrix operations
    pi_perf_reset();
    pi_perf_start();

    /**************************************************************************
                                Task to profile
    ***************************************************************************/
    int i_curr=0;   // index of the current output vector row under processing
    int i_prev=0;   // index of the previous output vector row under processing

    int buffer_id = 0;  // id of the temporary buffer: can be 0 or 1

    // intialize output
    int L2_min = 1<<30; //initalize to the larger value
    int found_idx = -1;

    // read the first block of data (blocking function!)
    pi_ram_read(&ram, dataset_L3+0, temp_buffer, (uint32_t) Nb_BYTES);
    i_curr++;


    for(i_curr; i_curr<N_BLOCK; i_curr++,i_prev++){
       // printf("Going to read the block %d\n", i_curr);
        // compute the buffer ID as the last bit of the i_curr
        // *   buffer_id: used to store the next read
        // * 1-buffer_id: used to compute
        buffer_id = i_curr & 0x1;   

        // launch the read of the next block
        pi_ram_read_async(&ram, 
            dataset_L3+(i_curr*Nb_BYTES), 
            &temp_buffer[Nb*buffer_id], 
            (uint32_t) Nb_BYTES, 
            pi_task_callback(&ram_read_task, end_of_read, NULL));
        
        // compute the L2 distance
        buffer_to_compute = (short int *)&temp_buffer[Nb*(1-buffer_id)];
        for(int i = 0; i<2*Nb; i=i+2){
            x = buffer_to_compute[i+1];
            y = buffer_to_compute[i];
            dist_x = (x - xm);
            dist_y = (y - ym);
            L2 = dist_x*dist_x + dist_y*dist_y;
            if (L2<L2_min){
                L2_min = L2;
                found_idx = Nb*i_prev + i>>1;
            }
        }        
        
       // printf("Going to read the block %d\n", i_curr);

        // wait the previous transfer to complete before launching the next one
        while((ram_returns) != i_curr) {
            pi_yield(); // go to idle if not until an event (enf of write) is detected
        }
    }

    // process the last data
    buffer_to_compute = (short int *)&temp_buffer[Nb*buffer_id];
    for(int i = 0; i<2*Nb; i=i+2){
        x = buffer_to_compute[i+1];
        y = buffer_to_compute[i];
        dist_x = (x - xm);
        dist_y = (y - ym);
        L2 = dist_x*dist_x + dist_y*dist_y;
        if (L2<L2_min){
            L2_min = L2;
            found_idx = Nb*i_prev + i>>1;
        }
    }      
    /**************************************************************************
                                End of Task
    ***************************************************************************/
    // stop the performance counters
    pi_perf_stop(); 

    printf("Minimal L2 of %d at position %d\n", L2_min,  found_idx);

    // collect an print statistics
    uint32_t instr_cnt = pi_perf_read(PI_PERF_INSTR);
    uint32_t cycles_cnt = pi_perf_read(PI_PERF_CYCLES);
    float cpi = (float) cycles_cnt / instr_cnt;

    printf("Number of Instructions: %d\nClock Cycles: %d\nCPI: %f\n", 
        instr_cnt, cycles_cnt, cpi);


    // return 0 
    pmsis_exit(0);
}
