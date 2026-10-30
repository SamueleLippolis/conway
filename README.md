# Conway Game of Life - C and MPI
This small project implements Conway Game of Life in C, using MPi for the parallelization. 

## Execution 

### Compilation and execution 
Compile the project using make (you need to have "mpicc").
Alternatively, run 
- mpicc -Wall -Wextra -O2 main_mpi.c conway.c -o conway 600 200 1
Where the arguments are 
- N = size of the matrix
- steps = number of generations
- mode = 1 is pseudorandom, 0 is blinker 

### Performance Analysis
- Run `bash benchmark.sh` to measure execution times with 1, 2, and 4 MPI processes, repeating each experiment three times.
- Run `python analyze_benchmark.py` to compute median execution times, speedup, and parallel efficiency, and generate the corresponding plots.

## Description

### Parallel Implementation
The N x N grid is divided into P horizontal subdomains, each containing N/P rows (N must be divisible by P).
Each process exchanges boundary data with its periodic neighbors using two halo rows and `MPI_Sendrecv`.
Double buffering prevents overwriting the current state, while `MPI_Gather` collects the final grid on rank 0.


### Files description

- main.c: conway without parallelization
- main_mpi.c: complete mpi version of conway
- conway.c: functions of conway 
