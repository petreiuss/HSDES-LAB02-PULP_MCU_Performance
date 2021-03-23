# Double Buffering Example with L3 external memory

This example computes a vectorial product 
<img src="https://render.githubusercontent.com/render/math?math=A\mathrm{x}B=C">
where 
<img src="https://render.githubusercontent.com/render/math?math=A\in\mathrm{Z}^{N\mathrm{x}1},B\in\mathrm{Z}^{1\mathrm{x}N}">
and 
<img src="https://render.githubusercontent.com/render/math?math=C\in\mathrm{Z}^{N\mathrm{x}N}"> 
and Z indicating integer numbers.
Because of memory contraints, when N is high (e.g. 512) the vector _C_ does not fit the L2 on-chip memory and therefore must be stored within an external memory.
On the contrary, _A_ and _B_ vectors are stored in the embedded L2 memory.

## Getting Started
To get started with the example, simply:
~~~~~shell
cd double_buffering/
make clean all run
~~~~~
The code makes use of the PMISIS APIs to allocate variables on the external RAM and to perform R/W operations. 
You can find more info [here](https://greenwaves-technologies.com/manuals/BUILD/PMSIS_BSP/html/group__Ram.html).

## VCD Traces Visualization
To undestand what it is happening at system level we can visualize the [VCD Traces](https://greenwaves-technologies.com/manuals/BUILD/GVSOC/html/index.html). 
The VCD Traces show the state of several components over the time, like the cores PC, the DMA transfers, etc, and thus gives a better overview than the system traces. 
[Here](https://gvsoc.readthedocs.io/en/latest/vcd_traces.html) you can find more details.
To this aim:
~~~~~shell
make clean all run runner_args="--vcd"
gtkwave BUILD/PULP/GCC_RISCV/view.gtkw
~~~~~
GTKwave is a tool that can be used to visualize the generated traces. 
Hint: simply copy-paste and execute the command returned by the GVSOC simulation.

**Q:** What is the percentage of active cycles?

## Double Buffering
If you run:
~~~~~shell
make clean all run DB=1
~~~~~
Do you see any improvements in terms of clock cycles? Why?

