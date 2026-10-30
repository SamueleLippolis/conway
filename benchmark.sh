#!/bin/bash
N=$(awk '$2 == "DEFAULT_N" {print $3}' config.h)
steps=$(awk '$2 == "DEFAULT_STEPS" {print $3}' config.h)
mode=$(awk '$2 == "DEFAULT_MODE" {print $3}' config.h)

echo "processes,run,time_s" > ./reports/benchmark_N${N}_steps${steps}_mode${mode}.csv

for p in 1 2 4; do 
    for run in 1 2 3; do
        time=$(mpirun -np "$p" ./conway_mpi ${N} ${steps} ${mode} | awk '{print $(NF-1)}')
        echo "$p,$run,$time" >> ./reports/benchmark_N${N}_steps${steps}_mode${mode}.csv
    done
done